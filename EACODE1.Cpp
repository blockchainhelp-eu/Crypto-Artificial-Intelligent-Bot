
#include <stdlib.mqh>
#include <stderror.mqh>
#include <WinUser32.mqh>

#include <Inform.mqh>                    
#include <OrderAccounting.mqh>           
#include <Events.mqh>                    
#include <Trade.mqh>                   

#include <TradeLogger.mqh>
#include <TradingCriterionSkeleton.mqh>  

int init()                             
{
   initInformation();        
   initOrderAccounting();    
   initEvents();            
   initTradingModule();
int lastOpenedTicketNr;

int trade(int operation)
{
   if(operation==-2) // Do nothing
      return(0);
   if(operation==-1)
      manageOpenOrders();
   else
      openOrderByType(operation);
   return(0);
}

void openOrderByType(int operation)
{
   inform(1,0,0, "Order request.. " + operation);
   for(int part=0;part<orderParts;part++)
   {
      bool orderOpened=false; int retryCounter=0;
      
      
   
   initTradeLogger();
   
   return(0);
}

int start()                        
{
   if(!evaluateAtOpen || (evaluateAtOpen && newBar()))
   {
      orderAccounting();
      tradingConditionEvents();                       
      trade(criterion());       
      inform(0);
   }
   tradeLogger();
   
   return(0);
#include <stdlib.mqh>
#include <stderror.mqh>
#include <WinUser32.mqh>
int init()                             
{
   initInformation();        
         orderOpened=true;
         
         double step = MarketInfo(Symbol(),MODE_LOTSTEP);
         double newLots = MathFloor(getInitialLotsize(operation,part+1)/step)*step;
         if(!volumeCheck(newLots))
            break;
         
         double price = NormalizeDouble(getInitialOpeningPrice(operation, part+1),Digits);      
         double stoploss = NormalizeDouble(getInitialStoplossLevel(operation, part+1),Digits);
         double takeProfit = NormalizeDouble(getInitialTakeProfitLevel(operation, part+1),Digits);
         datetime expiration = getInitialExpirationDate(operation, part+1);
         
         //Print("Price: ", DoubleToStr(price,Digits));
         //Print("Stoploss: ", DoubleToStr(stoploss,Digits));
         //Print("takeProfit: ", DoubleToStr(takeProfit,Digits));
         
         setCurrentOrderPart(part+1);
         int tradeResult=TRADE_OPERATION_HALT;
         
     
    else        // Place order without SL/TP and add SL/TP through modification
         {
            tradeResult=openOrder(operation,newLots,price,0,0,expiration);
            if(tradeResult==TRADE_OPERATION_OK)     // Trade without SL/TP placed succesfully
            {
               bool modificationOk=false; int retryCounterECN=0;
               while(!modificationOk)     // Modify: add the SL/TP
               {
                  modificationOk=true;
                  tradeResult=modifyOrder(lastOpenedTicketNr,price,stoploss,takeProfit,expiration);
                  
                  if(tradeResult==TRADE_OPERATION_RETRY)  // Retry modification
                  {
                     if(retryCounterECN<3) // Retry the modification of the order
                        modificationOk=false;
                     else                 // Cancel the order   
                     {
                        inform(15,999);
                        tradeResult=TRADE_OPERATION_HALT;
                     }
                     retryCounterECN+=1;
                  }
                  
                  if(tradeResult==TRADE_OPERATION_HALT)   // Modification failed
                  {
                     //inform(27,lastOpenedTicketNr);         // Could not add SL/TP. Delete original order
                     bool deleteOk=false;
                     while(!deleteOk)
                     {
                        deleteOk=true;
                        tradeResult=closeOrDeleteOrder(operation,lastOpenedTicketNr,newLots);
                        if(tradeResult==TRADE_OPERATION_RETRY)  // Retry removing the original order, ad infinum
                           deleteOk=false;
                        if(tradeResult==TRADE_OPERATION_HALT)   // Critical: could not remove original order without SL/TP
                           inform(23,lastOpenedTicketNr);
                     }
                  }
               }
            }
         }
         
         //------------------------------------- Retry placement of trade if necessary ------------//
         if(tradeResult==TRADE_OPERATION_RETRY)
         {
            if(retryCounter<3)
               orderOpened=false;  
            else
            {
               inform(15,999);             
               tradeResult=TRADE_OPERATION_HALT;
            }
            retryCounter+=1;
         }   
         if(tradeResult==TRADE_OPERATION_HALT)   // Critical error: break
		 {
			inform(1,0,0, "Trade operation halt");
            break;
		 }
      }
   }                         
   return;
}


void manageOpenOrders()
{
   double oldPrice, oldTakeProfit, oldStoploss, oldLots;
   datetime oldExpirationDate;
   double newPrice, newTakeProfit, newStoploss, newLots;
   datetime newExpirationDate;
   
   int operation, ticket;

   for(int i=0; i<nrOrders;i++)
   {
      bool orderManaged=false; int retryCounter=0;
      while(!orderManaged)
      {
         orderManaged=true;
         int result=TRADE_OPERATION_HALT;
         
         operation = newOrders[i][5];
         ticket = newOrders[i][3];
         newLots = newOrders[i][4];
      
         if(closePosition(i))
            result = closeOrDeleteOrder(operation,ticket,newLots);
         else
         { 
            double step = MarketInfo(Symbol(),MODE_LOTSTEP);
            oldPrice = NormalizeDouble(newOrders[i][0],Digits);
            oldTakeProfit = NormalizeDouble(newOrders[i][2],Digits);
            oldStoploss = NormalizeDouble(newOrders[i][1],Digits);
            oldLots = MathFloor(newOrders[i][4]/step)*step;
            oldExpirationDate = newOrders[i][9];
      
            newPrice = NormalizeDouble(getNewOpeningPrice(i),Digits);
            newStoploss = NormalizeDouble(getNewStoplossLevel(i), Digits);
            newTakeProfit = NormalizeDouble(getNewTakeProfitLevel(i), Digits);
            newLots = MathFloor(getNewLotsize(i)/step)*step;
            newExpirationDate = getNewExpirationDate(i);
      
            if(oldPrice!=newPrice || oldTakeProfit!=newTakeProfit 
               || oldStoploss!=newStoploss || oldExpirationDate!=newExpirationDate)
            {
               result = modifyOrder(ticket,newPrice, newStoploss, newTakeProfit, newExpirationDate);
            }
            if(oldLots>newLots)  // Partly close order
            {
               double closingPrice = Bid;
               if(operation==OP_SELL)
                  closingPrice=Ask;
               if(oldLots-newLots >= MarketInfo(Symbol(),MODE_MINLOT))
                  result = closeOrder(ticket,oldLots-newLots,closingPrice);
               else
                  result = closeOrder(ticket,oldLots,closingPrice);
            }
         }
         
         if(result==TRADE_OPERATION_RETRY)
         {
            if(retryCounter < 3)
               orderManaged=false;
            else
            {
               inform(15,999);
               result=TRADE_OPERATION_HALT;
            }
            retryCounter+=1;
         }
		 
		 if(result==TRADE_OPERATION_HALT)
		 {
			break;
		 }
      }  
   }
}
 
int closeOrDeleteOrder(int operation,int ticketNr,double lotSize)
{
   if(operation==OP_BUY ||operation==OP_SELL)
   { 
      double closingPrice = Bid;
      if(operation==OP_SELL)
         closingPrice = Ask;
      return(closeOrder(ticketNr,lotSize,closingPrice));
   }
   else
      return(deleteOrder(ticketNr));
}


   initEvents();                  
   initTradingModule();          
   
   initTradeLogger();
   
   return(0);
int openOrder(int operation, double newLots, double price, double stoploss, double takeProfit, datetime expiration)
{
   inform(13,operation);
   lastOpenedTicketNr=OrderSend(Symbol(),operation,newLots,price,allowedSlippage,stoploss,takeProfit,"",EAUniqueID,expiration);
   if(lastOpenedTicketNr<0)
   {
      if(handleErrors(GetLastError())==false)   // Check for critical error
         return(TRADE_OPERATION_HALT);
      return(TRADE_OPERATION_RETRY);
   }
   orderAccounting();
   orderEvents();
   return(TRADE_OPERATION_OK);
}
  

bool closeOrder(int ticketNr, double lotSize, double closingPrice)
{
   inform(12,ticketNr);
   if(!OrderClose(ticketNr,lotSize,closingPrice,allowedSlippage))
   {
      if(handleErrors(GetLastError())==false)
         return(TRADE_OPERATION_HALT);
      return(TRADE_OPERATION_RETRY);
   }
   orderAccounting();
   orderEvents();
   return(TRADE_OPERATION_OK);
}

bool deleteOrder(int ticketNr)
{
   inform(22,ticketNr);
   if(!OrderDelete(ticketNr))
   {
      if(handleErrors(GetLastError())==false)
         return(TRADE_OPERATION_HALT);
      return(TRADE_OPERATION_RETRY);
   }
   orderAccounting();
   orderEvents();
   return(TRADE_OPERATION_OK);
}

bool modifyOrder(int ticketNr,double newPrice,double newStoploss,double newTakeProfit,datetime newExpirationDate)
{
   inform(19,ticketNr);
   if(!OrderModify(ticketNr, newPrice, newStoploss, newTakeProfit, newExpirationDate))          
   {                               
      if(handleErrors(GetLastError())==false)
         return(TRADE_OPERATION_HALT);                
      return(TRADE_OPERATION_RETRY);
   }
   orderAccounting();
   orderEvents();
   return(TRADE_OPERATION_OK);
}

//****************************************************************************************************************//
//********************************************* Error handling ***************************************************//
//****************************************************************************************************************//

bool handleErrors(int error)                   
{
   inform(15, error);                     // Message
   switch(error)
   { 
      case 129:                           // Wrong price
      case 135:                           // Price changed
      case 138:                           // Requote
         RefreshRates();                
         return(true);                   
      case 136:                           // No quotes. Waiting for the tick to come
         while(RefreshRates()==false)    
            Sleep(1);                     
         return(true);
      case 4  :                           // Trade Server is busy                  
      case 6  :                           // No connection to the server
      case 128:                           // Trade Timeout
      case 137:                           // Broker is busy
      case 146:                           // The trade subsystem is busy
         Sleep(500);                     
         RefreshRates();                 
         return(true);                    
                                          // Critical errors:
      case 2 :                            // Common error
      case 5 :                            // Old version of the client terminal
      case 7 :                            // Not enough rights
      case 64:                            // Account blocked
      case 133:                           // Trading is prohibited
      case 141:                           // Too many requests
      case 148:                           // Too many orders
      case 0  :                           // Logical error in trading logic (Most likely..)
      default:                            // Other variants
         return(false);                   
     }
}

//***********************************************************//
// Helper functions..                                        //
//***********************************************************//
bool volumeCheck(double newLots)
{
   double minLot = MarketInfo(Symbol(),MODE_MINLOT);
   if (newLots < minLot)
   {
      inform(11,1,newLots);  // Lotsize too small!
      return(false);
   }
   
   double maxLot = MarketInfo(Symbol(),MODE_MAXLOT);   
   if (newLots > maxLot)
   {
      inform(11,2,newLots);
      return(false);
   }     
   
   double freeMargin = AccountFreeMargin();  
   double marginPerLot = MarketInfo(Symbol(),MODE_MARGINREQUIRED);          
   if (newLots*marginPerLot>freeMargin) //not enough margin
   {                                       
      inform(11,0,newLots); // Not enough money!                 
      return(false);                           
   }
   
   return(true);
}

/**
*  Check the minimum distance and the freezelevel between a baseline and the requestedlevel.
*  eg: Baseline for sell order = ASK, Baseline for buy order = BID
*  If requestedLevel too close: Return closest possible price, otherwise return requestedLevel
*/
double checkMinDistanceAndFreezeLevel(double baseline, double requestedLevel)
{
   double requiredMinOffset = MathMax(newLevel,newFreeze)*Point;
   if(baseline>requestedLevel && baseline-requiredMinOffset<requestedLevel)
   {
      Print("New level (baseline>requestedLevel)");
      return(baseline-requiredMinOffset); // RequestedLevel too close
   }
   if(baseline<requestedLevel && baseline+requiredMinOffset>requestedLevel)
   {
      Print("New level (baseline<requestedLevel");
      return(baseline+requiredMinOffset); // RequestedLevel too close
   }
      
   return(requestedLevel); // RequestedLevel OK
}
}

int start()                        
{
   if(!evaluateAtOpen || (evaluateAtOpen && newBar()))
   {
      orderAccounting();
      tradingConditionEvents();                       
      trade(criterion());       
      inform(0);
   }
   tradeLogger();
   
   return(0);
}  

int deinit()                      
{
   orderAccounting();
   deinitTradingModule();
   inform(-1); 
   
   deinitTradeLogger();      
    
   return(0);                       
}

bool newBar()
{
   static datetime latestBar = 0;
   if(Time[0] != latestBar)
   {
      latestBar = Time[0];
      return(true);
   }  
   return(false);
}
}  
void tradingConditionEvents()
{
	newLevel=MarketInfo(Symbol(),MODE_STOPLEVEL); // Last known mindistance
   newFreeze=MarketInfo(Symbol(),MODE_FREEZELEVEL);
   spreadBA=MarketInfo(Symbol(),MODE_SPREAD);
   if (oldLevel!=newLevel)          
   {                                   
      oldLevel=newLevel;                 // New "old value"
      inform(10,newLevel);               // Message: new distance
   }
   if(oldFreeze!=newFreeze)
   {
      oldFreeze=newFreeze;
      inform(28,newFreeze);
   }
}

void orderEvents()
{
	if(checkEvents)
	{
		checkModifiedOrders();
		checkNewOrders();
	}
}

//---------------------------------------------------------------------
void checkModifiedOrders()
{
   bool match;                           
   // Searching for lost, type-changed, partly closed and reopened orders   
   for(int old=0; old<nrOldOrders; old++)
   { 
      match=false;
      for(int neww=0; neww<nrOrders; neww++)
      {
         if(checkTypeChanged(old, neww)) // Search for type changed orders
         {
            match=true; break;
         }
         if(checkReopenedOrPartlyClosed(old, neww)) // Search for reopened or partly closed orders
         {
            match=true; break;
         }                               
      }
      if (!match)               // Order was not found (closed)
         checkClosedOrder(old);     
    }
}

bool checkTypeChanged(int old, int neww)
{
   if (oldOrders[old][3]==newOrders[neww][3]) // Same ticket
   {                              
      if (newOrders[neww][5]!= oldOrders[old][5]) // Type changed?
         inform(7, newOrders[neww][3]);
      return(true);                     
   }
   return(false);
}

bool checkReopenedOrPartlyClosed(int old, int neww) 
{
   
   if (newOrders[neww][7]>0) // Check for ordercomment
                            // Server places old ticketnumber in comment when reopened or partly closed
   {
      OrderSelect(newOrders[neww][3], SELECT_BY_TICKET);                    //get comment
      if(StringFind(OrderComment(), DoubleToStr(oldOrders[old][3],0))!=-1) //try to find old ticketnumber in comment
      {
         if (oldOrders[old][4]==newOrders[neww][4])  // Same volume
            inform(8,oldOrders[old][3]);            // REOPENED (broker glitch)
         else                           
            inform(9,oldOrders[old][3]);            // PARTLY CLOSED
         return(true);
      }                                 
   }
   return(false);
}

void checkClosedOrder(int old)
{
   if (oldOrders[old][5]==0)
      inform(1, oldOrders[old][3]);  // Order Buy closed
   if (oldOrders[old][5]==1)
      inform(2, oldOrders[old][3]);  // Order Sell closed
   if (oldOrders[old][5]> 1)
      inform(3, oldOrders[old][3]);  // Pending order deleted
}


void checkNewOrders()
 {
      if(messageNr!=0 && explicitMessage=="" && printDebugInfo)
         Print(getMessage(messageNr,number,value));
      if(messageNr!=0 && explicitMessage!="" && printDebugInfo)
         Print(explicitMessage);
      return;
   }

   int    windowNr;                    // Indicator window number
   string message;                     // Message line
   static int    messageTime;          // Last publication time of the message
   static int    messageCounter;       // Graphical messages counter

   windowNr = WindowFind("Information");  // Searching for indicator window number
   if (windowNr < 0)
   {
      if(messageNr!=0 && explicitMessage=="" && printInfo)
         Print(getMessage(messageNr,number,value));
      if(messageNr!=0 && explicitMessage!="" && printInfo)
         Print(explicitMessage);
      return;

{
   for(int neww=0;neww<nrOrders;neww++)
   {
      // Ordercomment: reopened or partly closed order (not new)
      if (newOrders[neww][7]>0)
         continue;
         
      bool match=false;                
      for(int old=0;old<nrOldOrders;old++)       
      {                                 
         if (newOrders[neww][3]==oldOrders[old][3])   
         {                                         
            match=true;                      
            break;
         }                                
      }
      if (!match)                         
      {                                  
         if (newOrders[neww][5]==0)
            inform(4, newOrders[neww][3]); // Inform order Buy opened
         if (newOrders[neww][5]==1)
            inform(5, newOrders[neww][3]); // Inform order Sell opened
         if (newOrders[neww][5]> 1)
            inform(6, newOrders[neww][3]); // Inform Pending order placed
      }
   }
}

void initEvents()
{
   oldLevel = MarketInfo(Symbol(), MODE_STOPLEVEL);   // Set initial minimum distance
   oldFreeze = MarketInfo(Symbol(), MODE_FREEZELEVEL); // Set initial freezelevel
   spreadBA = MarketInfo(Symbol(), MODE_SPREAD);
   Print("Current bid ask Spread: ", DoubleToStr(spreadBA,Digits));
   Print("Current minimum distance: ", DoubleToStr(oldLevel,Digits));
   Print("Current freezelevel: ", DoubleToStr(oldFreeze,Digits));
}

int deinit()                      
{
   orderAccounting();
   deinitTradingModule();
   inform(-1); 
   
   deinitTradeLogger();      
    
   return(0);          
 string message;
   switch(messageNumber)
   {
      case 1:
         message="Closed order Buy  " + number;
         PlaySound("Close_order.wav");                         break;
      case 2:
         message="Closed order Sell " + number;
         PlaySound("Close_order.wav");                         break;
      case 3:
         message="Deleted pending order " + number;
         PlaySound("Close_order.wav");                         break;
      case 4:
         message="Opened order Buy " + number;
         PlaySound("Ok.wav");                                  break;
      case 5:
         message="Opened order Sell " + number;
         PlaySound("Ok.wav");                                  break;
      case 6:
         message="Placed pending order " + number;
         PlaySound("Ok.wav");                                  break;
      case 7:
         message="Order "+number+" modified into the market one";
         PlaySound("Transform.wav");                           break;
      case 8:
         message="Reopened order "+ number;                  break;
         PlaySound("Bulk.wav");
      case 9:
         message="Partly closed order "+ number;
         PlaySound("Close_order.wav");                         break;
      case 10:
         message="Broker: new minimum distance: "+ number;
         PlaySound("Inform.wav");                              break;
      case 11:
         if(number==0)
            message="Not enough money for "+DoubleToStr(value,3) + " lots";
         if(number==1)
            message="Required lotsize (" + DoubleToStr(value,3) + ") is too small";
         if(number==2)
            message="Required lotsize (" + DoubleToStr(value,3) + ") is too large"; 
         messageColor=Red;
         PlaySound("Oops.wav");                                break;
      case 12:
         message="Trying to close order " + number;
         PlaySound("expert.wav");                              break;
      case 13:
         if (number==OP_BUY || number==OP_SELL)
            message="Trying to open market order " + number;
         else
            message="Trying to open Pending order";
         PlaySound("expert.wav");                              break;
      case 14:
         message="Invalid password for live trading. EA does not function.";
         messageColor=Red;
         PlaySound("Oops.wav");                                break;
      case 15:
         switch(number)                 // Going to the error number
         {
            case 0  : message="Logical error. Contact strategycoder@gmail.com";                 break;
            case 4  : message="Trade server is busy. Retrying..";                               break;
            case 129: message="Wrong price. Retrying..";                                        break;
            case 135: message="Price changed. Retrying..";                                      break;
            case 136: message="No prices. Awaiting a new tick..";                               break;
            case 137: message="Broker is busy. Retrying..";                                     break;
            case 138: message="Requote. Retrying..";                                            break;
            case 146: message="Trading subsystem is busy. Retrying.. ";                         break;
            case 6 :  message="No connection to the server (check connection). Retrying..";     break;
            case 128: message="Trade timeout (check connection). Retrying..";                   break;
            case 2:   message="Critical: Common error.";                                        break;
            case 5 :  message="Critical: Old version of the terminal.";                         break;
            case 145: message="Critical: Order is to close to market price.";                   break;
            case 64:  message="Critical: Account is blocked.";                                  break;
            case 133: message="Critical: Trading is prohibited";                                break;
            case 8 :  message="Critical: Too frequent requests";                                break;
            case 7 :  message="Critical: Not enough rights to trade";                           break;
            case 148: message="Critical Too many open and/or pending orders";                   break;
            case 141: message="Critical: Too many requests";                                    break;
            case 999: message="Critical: Too many retrys";                                      break;
            default:  message="Critical error occurred " + number;   //Other errors
         }
         messageColor=Red;
         PlaySound("Error.wav");                                    break; 
      case 16:
         message="Extern variable risk must be greater than 0 and smaller than 100. EA does not function.";
         messageColor=Red;
         PlaySound("Oops.wav"); break;
      case 17:
         message="Fixed lotsize must be greater than the minmal lotsize. EA does not function.";
         messageColor=Red;
         PlaySound("Oops.wav"); break;
      case 18:
         message="extern variable freeMargin must be greater than 0 and smaller or equal to 100 when modevolume == 2. EA does not function.";
         messageColor=Red;
         PlaySound("Oops.wav"); break;
      case 19:
         message="Trying to modify order " + number;
         PlaySound("expert.wav"); break;
      case 20:
         message="Expert succesfully initialized! Start execution at next tick..";
         PlaySound("expert.wav"); break;
      case 21:
         message="Price difference between the current Bid/Ask price and the openingprice for the pending is to small!";
         PlaySound("Oops.wav"); break;
      case 22:
         message="trying to delete pending order" + number;
         PlaySound("expert.wav"); break;
      case 23:
         message="Critical! Order " + number + " could not be removed (No SL/TP placed!)";
         PlaySound("Error.wav");
      case 24:
         message="Fixed lotsize must be smaller than the maximal lotsize. EA does not function.";
         messageColor=Red;
         PlaySound("Oops.wav"); break;
      case 25:
         message="Multiple options for volume control at the same time. EA does not function";
         messageColor=Red;
         PlaySound("Oops.wav"); break;
      case 26:
         message="Distance between price and pending price too small";
         messageColor=Red;
         PlaySound("Oops.wav"); break;
      case 27:
         message="Unable to add SL/TP for order " + number + ": Trying to delete order..";
         messageColor=Red;
         PlaySound("Oops.wav"); break;
      case 28:
         message="Broker: new freezelevel: "+ number;
         PlaySound("Inform.wav");                              break;
      case -1:
         message="Deinit";
         messageColor=Red;
         PlaySound("Expert.wav"); break;
      default:
         message="default "+ messageNumber;
         messageColor=Red;
         PlaySound("Bzrrr.wav");
   }
   return(message);  
}

void initInformation()
{
   for(int i=0;i<30;i++)
   {
      messageArray[i]="";
   }
}             
}

bool newBar()
{
   static datetime latestBar = 0;
   if(Time[0] != latestBar)
   {
      latestBar = Time[0];
      return(true);
   }  
   return(false);
}