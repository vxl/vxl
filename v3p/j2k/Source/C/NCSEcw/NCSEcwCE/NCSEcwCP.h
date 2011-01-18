#ifndef _NCSECWCP_H_
#define _NCSECWCP_H_



template <class T>
class CProxy_INCSRendererEvents : public IConnectionPointImpl<T, &DIID__INCSRendererEvents, CComDynamicUnkArray>
{
	//Warning this class may be recreated by the wizard.
public:
	VOID Fire_RefreshUpdate(DOUBLE dWorldTLX, DOUBLE dWorldTLY, DOUBLE dWorldBRX, DOUBLE dWorldBRY, LONG nWidth, LONG nHeight)
	{
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		CComVariant* pvars = new CComVariant[6];
		int nConnections = m_vec.GetSize();
		
		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			IDispatch* pDispatch = reinterpret_cast<IDispatch*>(sp.p);
			if (pDispatch != NULL)
			{
				pvars[5] = dWorldTLX;
				pvars[4] = dWorldTLY;
				pvars[3] = dWorldBRX;
				pvars[2] = dWorldBRY;
				pvars[1] = nWidth;
				pvars[0] = nHeight;
				DISPPARAMS disp = { pvars, NULL, 6, 0 };
				pDispatch->Invoke(0x1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
		}
		delete[] pvars;
	
	}
};
#endif
