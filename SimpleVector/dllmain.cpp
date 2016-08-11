/* Replace "dll.h" with the name of your header */
#include "SimpleVector.h"
#include "OrderInfo.h"

SimpleVector<OrderInfo> orderlist;
int CreateManager()
{
 	OrderInfo info;
 	info.type = 3;
 	info.cmd = 34;
 	orderlist.push(info);
 	return 0;
}

int getOrder(OrderInfo &info, int index)
{
 	info = orderlist[0];
 	return 0;
}

int getCount()
{
 	return orderlist.size();
}
