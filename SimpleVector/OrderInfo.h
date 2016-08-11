#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

struct OrderInfo
{
 	   int type;
 	   
 	   int cmd;
};
int DLLIMPORT CreateManager();

int DLLIMPORT getOrder(OrderInfo &info, int index);

int DLLIMPORT getCount();
