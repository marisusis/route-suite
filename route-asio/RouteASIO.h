#ifndef ROUTE_SUITE_ROUTEASIO_H
#define ROUTE_SUITE_ROUTEASIO_H


#include <atlbase.h>
#include <atlcom.h>
#include "iasiodrv.h"

namespace Route {

    class RouteASIO : public IASIO, public CUnknown {
    public:
        RouteASIO(LPUNKNOWN pUnk, HRESULT* phr);

        ~RouteASIO();

        DECLARE_IUNKNOWN
        //STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {      \
    //    return GetOwner()->QueryInterface(riid,ppv);            \
    //};                                                          \
    //STDMETHODIMP_(ULONG) AddRef() {                             \
    //    return GetOwner()->AddRef();                            \
    //};                                                          \
    //STDMETHODIMP_(ULONG) Release() {                            \
    //    return GetOwner()->Release();                           \
    //};

        // Factory method
        static CUnknown* CreateInstance(LPUNKNOWN pUnk, HRESULT* phr);

        // IUnknown
        virtual HRESULT STDMETHODCALLTYPE NonDelegatingQueryInterface(REFIID riid, void** ppvObject);



    class __declspec(uuid("a8494b3c-d061-4814-8567-1b95028c2d72")) RouteASIO
            : public IASIO, public IRouteASIO, public CComObjectRootEx<CComMultiThreadModel>,
              public CComCoClass<RouteASIO, &__uuidof(RouteASIO)> {

    BEGIN_COM_MAP(RouteASIO)
                            COM_INTERFACE_ENTRY(IRouteASIO)

                            // To add insult to injury, ASIO mistakes the CLSID for an IID when calling CoCreateInstance(). Yuck.
                            COM_INTERFACE_ENTRY(RouteASIO)

                            // IASIO doesn't have an IID (see above), which is why it doesn't appear here.
        END_COM_MAP()




    };

}


#endif //ROUTE_SUITE_ROUTEASIO_H
