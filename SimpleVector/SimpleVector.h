#ifndef SIMPLEVECTOR_H_
#define SIMPLEVECTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#define _CAPACITY_RATE_ 1.1

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */



typedef int (*cmp_fn)(const void*, const void*);

template<class T>
class DLLIMPORT SimpleVector {
private:
	//value list
	T * m_list;
	size_t m_size;
	//posize_t list of value
	T** m_sortPointList;
	size_t m_sortSize;
	//
	T** m_cachePointList;
	size_t m_cacheSize;
	size_t m_maxSize;
	//

	bool relocate(size_t extendToSize) {
		T* newData = new T[extendToSize];
		if (newData == NULL) {
			return false;
		}
		T** newDataPosize_t = new T*[extendToSize];
		if (newDataPosize_t == NULL) {
			delete[] newData;
			return false;
		}
		T** newCachePosize_t = new T*[extendToSize];
		if (newCachePosize_t == NULL) {
			delete[] newDataPosize_t;
			delete[] newData;
			return false;
		}
		m_size = 0;

		if(m_sortSize > 0 && m_list != NULL){
			for(size_t j = 0; j < m_sortSize; j++){
				newData[j] = *(m_sortPointList[j]);
				m_size++;
			}
		}

		for (size_t j = 0; j < extendToSize; j++) {
			newDataPosize_t[j] = newData + j;
		}
		if (m_sortPointList != NULL)
			delete[] m_sortPointList;
		if (m_list != NULL)
			delete[] m_list;
		if(m_cachePointList != NULL)
			delete[] m_cachePointList;

		m_list = newData;
		m_sortPointList = newDataPosize_t;
		m_cachePointList = newCachePosize_t;
		m_cacheSize = 0;
		m_maxSize = extendToSize;
		return true;
	}

	bool copy(const SimpleVector & par){
		if (m_maxSize < par.m_sortSize) {
			if (!relocate(par.m_sortSize)) {
				return false;
			}
		} else {
			for (size_t i = 0; i < m_size; i++) {
				m_list[i].~T();
			}
		}

		for (size_t i = 0; i < par.m_sortSize; i++) {
			m_list[i] = *(par.m_sortPointList[i]);
			m_sortPointList[i] = &m_list[i];
		}
		m_size = par.m_size;
		m_sortSize = par.m_sortSize;
		m_cacheSize = 0;
		return true;
	}
	T** getSortPointList() {
		return m_sortPointList;
	}

public:
	static int compare_int(const void* arg1, const void* arg2) {
		int** a = (int**) arg1;
		int** b = (int**) arg2;
		return (**a) - (**b);
	}

	static int compare_long(const void* arg1, const void* arg2) {
		long** a = (long**) arg1;
		long** b = (long**) arg2;

		if((**a) == (**b)) {
			return 0;
		}
		if((**a) < (**b)) {
			return -1;
		}

		return 1;
	}

	static int compare_SimpleVector(const void* arg1, const void* arg2) {
		T** a = (T**) arg1;
		T** b = (T**) arg2;

		return (**a).compare(**b);
	}

	bool erase(size_t index)
	{
		if(index >= m_sortSize || index < 0){
			return false;
		}

		m_cachePointList[m_cacheSize] = m_sortPointList[index];
		m_cacheSize++;

		for(int i = index; i < m_sortSize - 1; i++){
			m_sortPointList[i] = m_sortPointList[i + 1];
		}
		m_sortSize--;

		return true;
	}

	bool erase(size_t from, size_t to)
	{
		if(to >= m_sortSize || to < 0 || from < 0 || from > to){
			return false;
		}

		size_t eraseCount = to - from + 1;
		for(size_t i = 0; i < eraseCount; i++){
			m_cachePointList[m_cacheSize] = m_sortPointList[from + i];
			m_cacheSize++;
		}

		for(size_t i = 0; i < eraseCount && (to + i) < m_sortSize && (from + i) < m_sortSize - 1; i++){
			m_sortPointList[from + i] = m_sortPointList[to + 1 + i];
		}

		m_sortSize -= (to - from + 1);
		return true;
	}

	SimpleVector() {
		m_maxSize = 0;
		m_size = 0;
		m_list = NULL;
		m_sortPointList = NULL;
		m_sortSize = 0;
		m_cachePointList = NULL;
		m_cacheSize = 0;
	}

	bool init(size_t initSize) {
		//forbid relocate
		if(initSize == 0){
			return true;
		}
       if(m_size != 0 || m_list !=0){
			throw -1;
			return false;
       }

	    m_list = new T[initSize];
		if (m_list == NULL) {
			return false;
		}
		m_sortPointList = new T*[initSize];
		if (m_sortPointList == NULL) {
			delete[] m_list;
			return false;
		}
		m_maxSize = initSize;

		m_cachePointList = new T*[initSize];
		if (m_cachePointList == NULL) {
			delete[] m_list;
			delete[] m_sortPointList;
			return false;
		}
		m_cachePointList = 0;

		for (size_t i = 0; i < initSize; i++) {
			m_sortPointList[i] = m_list + i;
			m_size++;
			m_sortSize++;
		}
		return true;
	}


	SimpleVector & operator=(const SimpleVector & par) {
		copy(par);
		return *this;
	}

	SimpleVector(const SimpleVector & par) {
		m_maxSize = 0;
		m_size = 0;
		m_list = NULL;
		m_sortPointList = NULL;
		m_sortSize = 0;
		m_cachePointList = NULL;
		m_cacheSize = 0;
		copy(par);   //forbid =,Recursion
	}

	explicit SimpleVector(size_t initSize) {
		m_maxSize = 0;
		m_size = 0;
		m_list = NULL;
		m_sortPointList = NULL;
		m_sortSize = 0;
		m_cachePointList = NULL;
		m_cacheSize = 0;
		relocate(initSize);
	}

	~SimpleVector() {
		if (m_list != NULL) {
			delete[] m_list;
			m_list = NULL;
		}
		if (m_sortPointList != NULL) {
			delete[] m_sortPointList;
			m_sortPointList = NULL;
		}
		if(m_cachePointList != NULL){
			delete[] m_cachePointList;
			m_cachePointList = NULL;
		}
	}
	void sort() {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_sortSize, (size_t) sizeof(T*), compare_SimpleVector);
	}

	void sort_by_int() {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_sortSize, (size_t) sizeof(T*), compare_int);
	}
	void sort_by_long() {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_sortSize, (size_t) sizeof(T*), compare_long);
	}

	void sort_by_fn(int (*cmp)(const void*, const void*)) {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_sortSize, (size_t) sizeof(T*), cmp);
	}

	bool push_back(const T &t){
		return push(t);
	}

	bool push(const T &t) {
		if(m_cacheSize > 0){
			*(m_cachePointList[m_cacheSize - 1]) = t;
			m_sortPointList[m_sortSize] = m_cachePointList[m_cacheSize - 1];
			m_cacheSize--;
			m_sortSize++;
			return true;
		}

		if (m_size == m_maxSize) {
			size_t newsize =
					((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) > m_size + 1 ?
							((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) : m_size + 1;
			if (!relocate(newsize)) {
				return false;
			}
		}
		*(m_list + m_size) = t;
		m_sortPointList[m_sortSize] = (m_list + m_size);
		m_size++;
		m_sortSize++;
		return true;
	}

	bool pushArray(T* t, size_t length) {
		size_t arryIndex = 0;
		if(m_cacheSize > 0){

			for (arryIndex = 0; arryIndex < length && m_sortSize > 0; arryIndex++) {
				*(m_cachePointList[m_cacheSize - 1]) = *(t + arryIndex);
				m_sortPointList[m_sortSize] = m_cachePointList[m_cacheSize - 1];
				m_cacheSize--;
				m_sortSize++;
			}
		}

		if (m_size + (length - arryIndex) > m_maxSize) {
			size_t newsize =
					((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) > m_size + length ?
							((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) : m_size + length;
			if (!relocate(newsize)) {
				return false;
			}
		}
		for (size_t i = arryIndex; i < length; i++) {
			*(m_list + m_size) = *(t + i);
			m_sortPointList[m_sortSize] = (m_list + m_size);
			m_sortSize++;
			m_size++;
		}

		return true;
	}

	T * find(const T &x) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), compare_SimpleVector);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}

	T * find_by_int(const T &x) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), compare_int);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}
	T * find_by_long(const T &x) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), compare_long);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}

	T * find_by_fn(const T &x, int (*cmp)(const void*, const void*)) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), cmp);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}

	int findPostion(const T &x) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;
		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), compare_SimpleVector);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}

	int findPostion_by_int(const T &x) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;
		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), compare_int);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}

	int findPostion_by_long(const T &x) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;
		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), compare_long);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}


	int findPostion_by_long(const T &x, int (*cmp)(const void*, const void*)) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;
		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_sortSize,
				(size_t) sizeof(T*), cmp);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}

	inline T * const at(const size_t index) {
		if (index > m_size - 1 || index < 0 || m_size==0) {
			return NULL;
		}
		return m_sortPointList[index];
	}

	inline size_t size() const{
		return m_sortSize;
	}
	inline size_t capacity() {
		return m_maxSize;
	}

	bool reserve(size_t toSize) {
		if (m_maxSize >= toSize) {
			return true;
		}
		return relocate(toSize);
	}

	void merge(const SimpleVector<T> & v) {
		if (v.m_sortSize > 0) {
			pushArray(v.m_list, v.m_sortSize);
		}
	}

	inline T & operator[](const size_t index) {
		if (index > m_size - 1 || index < 0 || m_size==0) {
			throw -1;
		}
		return *(m_sortPointList[index]);
	}
	inline const T & operator[](const size_t index) const{
		if (index > m_size - 1 || index < 0 || m_size==0 ) {
			throw -1;
		}
		return *(m_sortPointList[index]);
	}

	inline void setCount(const size_t count) {
		if (count > m_sortSize) {
			throw -1;
		}
		m_sortSize = count;
		return;
	}

	int findLowest(const T &x, int * lowest) {
		int finded_count = 0;
		int fined_pos = -1;
		*lowest = -1;
		if (m_sortSize <= 0) {
			return finded_count;
		}
		const T* p_x = &x;
		T** find_item = (T**) bsearch(&p_x, m_sortPointList, (size_t) m_sortSize, (size_t) sizeof(T*),
				compare_SimpleVector);
		if (find_item == NULL) {
			return finded_count;
		}
		finded_count = 1; //self count
		fined_pos = find_item - m_sortPointList;
		*lowest = fined_pos;
		while (*lowest > 0) {
			if (compare_SimpleVector(&(m_sortPointList[*lowest - 1]), &p_x) == 0) {
				*lowest = *lowest - 1;
				finded_count = finded_count + 1;
			} else {
				break;
			}
		}
		while (fined_pos < m_sortSize - 1) {
			if (compare_SimpleVector(&(m_sortPointList[fined_pos + 1]), &p_x) == 0) {
				fined_pos = fined_pos + 1;
				finded_count = finded_count + 1;
			} else {
				break;
			}
		}
		return finded_count;
	}
};

#endif /* SIMPLEVECTOR_H_ */
