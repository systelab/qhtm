/*----------------------------------------------------------------------
Copyright (c) 1998 Lee Alexander
Please see the file "licence.txt" for licencing details.
File:		Map.inl
Owner:	leea
Purpose:Implementation of CMap
----------------------------------------------------------------------*/



template<class Key, class Value>
CMap<Key, Value>::CMap( PI_uint32 uBucketArrayLength )
	:	m_pEntrys( NULL )
	,	m_uCount( 0 )
{
	SetBucketSize( uBucketArrayLength );
}

template<class Key, class Value>
CMap<Key, Value>::~CMap()
{
	RemoveAll();
	delete [] m_pEntrys;
	m_pEntrys = NULL;
}


template<class Key, class Value>
void CMap<Key, Value>::SetBucketSize( PI_uint32 uSize )
{
	_ASSERT( !GetSize() ); //Cannot be called unless map is empty (possible improvement would be to allow this).
	if( !GetSize() )
	{
		delete [] m_pEntrys;
		m_pEntrys = NULL;
	}

	m_uBucketArrayLength = uSize;
	m_pEntrys = new BucketEntry*[m_uBucketArrayLength];

	for( PI_uint32 i = 0; i < m_uBucketArrayLength; i++ )
	{
		m_pEntrys[i] = NULL;
	} 
}

template<class Key, class Value>
PI_uint32 CMap<Key, Value>::GetBucketSize() const
{
	return m_uBucketArrayLength;
}



template<class Key, class Value>
void CMap<Key, Value>::RemoveAll()
{
	if( m_uCount )
	{
		for( PI_uint32 i = 0; i < m_uBucketArrayLength; i++ )
		{
			BucketEntry *pEntry = m_pEntrys[i];
			while( pEntry )
			{
				BucketEntry *pEntryNext = pEntry -> m_pNext;
				delete pEntry;
				pEntry = pEntryNext;
			}

			m_pEntrys[i] = NULL;
		}

		m_uCount = 0;
	}
}

template<class Key, class Value>
CMap<Key, Value>::CMap( const CMap<Key, Value> &rhs )
	:	m_uCount( 0 )
	,	m_pEntrys( NULL )
{
	SetBucketSize( rhs.m_uBucketArrayLength );
	(void)operator = ( rhs );
}

template<class Key, class Value>
CMap<Key, Value> & CMap<Key, Value>::operator = ( const CMap<Key, Value> &rhs )
{
	if( this != &rhs )
	{
		RemoveAll();
		if( rhs.m_uCount )
		{
			for( PI_uint32 i = 0; i < rhs.m_uBucketArrayLength; i++ )
			{
				const BucketEntry *pEntry = rhs.m_pEntrys[i];
				while( pEntry )
				{
					SetAt( pEntry->m_Key, pEntry->m_Value );
					pEntry = pEntry -> m_pNext;
				}
			}
		}
	}

	return *this;
}

template<class Key, class Value>
Value *CMap<Key, Value>::Lookup( const Key &key )
{
	PI_uint32 uHash = HashIt( key ) % m_uBucketArrayLength;
	BucketEntry	*pEntry = Find( uHash, key );
	return pEntry != NULL ? &pEntry -> m_Value : NULL;
}


template<class Key, class Value>
bool CMap<Key, Value>::Lookup( const Key &key, Value &value )
{
	PI_uint32 uHash = HashIt( key ) % m_uBucketArrayLength;
	BucketEntry	*pEntry = Find( uHash, key );

	if( pEntry )
	{
		value = pEntry -> m_Value;
		return true;
	}

	return false;
}

template<class Key, class Value>
const Value * CMap<Key, Value>::Lookup( const Key &key ) const
{
	PI_uint32 uHash = HashIt( key ) % m_uBucketArrayLength;
	BucketEntry	*pEntry = Find( uHash, key );
	return pEntry != NULL ? &pEntry -> m_Value : NULL;
}


template<class Key, class Value>
typename CMap<Key, Value>::BucketEntry * CMap< Key, Value>::Find( PI_uint32 uHash, const Key &key ) const
{
	_ASSERT( m_pEntrys );

	for( BucketEntry *pEntry = m_pEntrys[uHash]; pEntry != NULL; pEntry = pEntry -> m_pNext )
	{
		if( ElementsTheSame( pEntry -> m_Key, key ) )
		{
			return pEntry;
		}
	}

	return NULL;
}

template<class Key, class Value>
Value &CMap<Key, Value>::CreateAt( const Key &key )
{
	PI_uint32 uHash = HashIt( key ) % m_uBucketArrayLength;
	BucketEntry	*pEntry = Find( uHash , key);

	if( pEntry )
	{
		RemoveAt( key ); // get rid of old entry
	}

	pEntry = new BucketEntry;
	m_uCount++;
	pEntry -> m_Key = key;

	BucketEntry	*pRootEntry = m_pEntrys[uHash];
	m_pEntrys[uHash] = pEntry;
	if( pRootEntry )
	{
		pEntry -> m_pNext = pRootEntry;
	}
	else
	{
		pEntry -> m_pNext = NULL;
	}
	return pEntry -> m_Value;
}

template<class Key, class Value>
void CMap<Key, Value>::SetAt( const Key &key, Value value )
{
	PI_uint32 uHash = HashIt( key ) % m_uBucketArrayLength;
	BucketEntry	*pEntry = Find( uHash , key);

	if( pEntry )
	{
		pEntry -> m_Value = value;
	}
	else
	{
		BucketEntry	*pEntry = new BucketEntry;
		m_uCount++;
		pEntry -> m_Value = value;
		pEntry -> m_Key = key;

		BucketEntry	*pRootEntry = m_pEntrys[uHash];
		m_pEntrys[uHash] = pEntry;
		if( pRootEntry )
		{
			pEntry -> m_pNext = pRootEntry;
		}
		else
		{
			pEntry -> m_pNext = NULL;
		}
	}
}

template<class Key, class Value>
void CMap<Key, Value>::RemoveAt( const Key &key )
{
	PI_uint32 uHash = HashIt( key ) % m_uBucketArrayLength;
	BucketEntry *pLastItem = NULL;
	for( BucketEntry *pEntry = m_pEntrys[uHash]; pEntry != NULL; pLastItem = pEntry, pEntry = pEntry -> m_pNext )
	{
		if( ElementsTheSame( pEntry -> m_Key, key ) )
		{
			if( pLastItem )
			{
				pLastItem -> m_pNext = pEntry -> m_pNext;
			}
			else
			{
				m_pEntrys[uHash] = pEntry -> m_pNext;
			}
			delete pEntry;
			m_uCount--;
			break;
		}
	}
}
