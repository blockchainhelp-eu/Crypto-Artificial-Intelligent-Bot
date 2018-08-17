
int sellMarket(double tradingLots,double stopLoss,double takeProfit,int expiration=0,color tradeColor=Red)
  {
   int ticket=OrderSend(Symbol(),OP_SELL,tradingLots,Bid,3,NormalizeDouble(stopLoss,Digits),NormalizeDouble(takeProfit,Digits),"Sell market trade",16384,expiration,tradeColor);
   if(ticket>0)
     {
      if(OrderSelect(ticket,SELECT_BY_TICKET,MODE_TRADES))
        {
         Print("SELL order opened : ",OrderOpenPrice());
        }
      return ticket;
     }
   else
     {
      Print("Error opening SELL order : ",GetLastError());
      return 0;
     }
  }
int modifyTrade(int ticket,double stopLoss,double takeProfit,int expiration=0,color modifyColor=Orange)
  {
   if(ticket>0)
     {
      if(OrderModify(ticket,OrderOpenPrice(),NormalizeDouble(stopLoss,Digits),NormalizeDouble(takeProfit,Digits),expiration,modifyColor))
        {
         Print("Order "+IntegerToString(ticket)+" modified");
         return ticket;
        }
     }
   else
     {
      Print("[ERROR] Could not modify trade as no ticket number was found");
     }

   return 0;
  }
//+------------------------------------------------------------------+
//| Close all open buy trades
//+------------------------------------------------------------------+
int closeAllBuys()
  {
   for(int i=0;i<OrdersTotal();i++)
     {
      if(!OrderSelect(i,SELECT_BY_POS,MODE_TRADES))
        {
         // could not find the order
         Print("[ERROR] Could not find the order to monitor stop loss");
         continue;
        }

      // if there is an open sell trade of this symbol
      if(OrderType()==OP_BUY && OrderSymbol()==Symbol())
        {
         int closeID=OrderClose(OrderTicket(),OrderLots(),Bid,3,Blue);

         if(!closeID)
           {
            Print("[ERROR] Could not close all buy trades | "+IntegerToString(GetLastError()));
            return -1;
           }
         else
           {
            return closeID;
           }
        }
     }
   return -1;
  }
//+------------------------------------------------------------------+
//| Close all open sell trades
//+------------------------------------------------------------------+
int closeAllSells()
  {
   for(int i=0;i<OrdersTotal();i++)
     {
      if(!OrderSelect(i,SELECT_BY_POS,MODE_TRADES))
        {
         // could not find the order
         Print("[ERROR] Could not find the order to monitor stop loss");
         continue;
        }

      // if there is an open sell trade of this symbol
      if(OrderType()==OP_SELL && OrderSymbol()==Symbol())
        {
         int closeID=OrderClose(OrderTicket(),OrderLots(),Ask,3,Blue);

         if(!closeID)
           {
            Print("[ERROR] Could not close all sell trades | "+IntegerToString(GetLastError()));
            return -1;
           }
         else
           {
            return closeID;
           }
        }
     }
   return -1;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int findSellCount()
  {
   int tradeCount=0;
   for(int i=0;i<OrdersTotal();i++)
     {
      if(OrderType()==OP_SELL && OrderSymbol()==Symbol())
        {
         tradeCount++;
        }
     }
   return tradeCount;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int findBuyCount()
  {
   int tradeCount=0;
   for(int i=0;i<OrdersTotal();i++)
     {
      if(OrderType()==OP_BUY && OrderSymbol()==Symbol())
        {
         tradeCount++;
        }
     }
   return tradeCount;
  }

//+------------------------------------------------------------------+
