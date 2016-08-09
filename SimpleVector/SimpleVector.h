#ifndef SIMPLEVECTOR_H_
#define SIMPLEVECTOR_H_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#include "dll.h"

template<class T>
class DLLIMPORT SimpleVector
{
  public:
    SimpleVector(int initSize){
					 this->m_data.init(initSize);
	 }
    ~SimpleVector(void);
	
  private:
  __SimpleVector<T> m_data;
};

#endif
