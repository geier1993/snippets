/*
 * IReaderExtension.hpp
 *
 *  Created on: 17.12.2018
 *      Author: Philipp Geier (PG)
 */

#include <cstddef> // NULL pointer for moderns GCC

#ifndef ICommonUtils_HPP_
#define ICommonUtils_HPP_

#if (__cplusplus > 201103L)
//! Only means partial C++ support, best is to have this Macro defined by Build Tool
#define CPP11SUPPORT
#endif

#if (__cplusplus > 201402L)
//! Only means partial C++ support, best is to have this Macro defined by Build Tool
#define CPP14SUPPORT
#endif

//! Helper for TypeDispatching. This (empty) construct is intented to only transport compile-time information.
template<typename T> struct TD {};
template<typename T> struct DummyBase {};

template<typename T, typename MemberType, typename GetType, typename ConstGetType, class Derived =void> 
class IGetter {
  public: 
    virtual ~IGetter() {}

    virtual GetType  get(const TD<T>&) =0;
    virtual ConstGetType  get(const TD<T>&) const =0;
};

template<typename T, typename MemberType, typename SetType, class Derived =void> 
class ISetter {
  public: 
    virtual ~ISetter() {}
    virtual void set(const TD<T>&, SetType) = 0;
};

template<typename T, typename MemberType, typename GetType, typename ConstGetType, typename SetType, class Derived =void> 
class IGetterSetter:
  public virtual IGetter<T,MemberType,GetType,ConstGetType,Derived>,
  public virtual ISetter<T,MemberType,SetType,Derived> {
  public: 
    virtual ~IGetterSetter() {}
};



template<typename T, class Derived =void> 
class IRefGetter: public virtual IGetter<T,T&,T&,const T&,Derived> {
  public: 
    virtual ~IRefGetter() {}
};

template<typename T, class Derived =void> 
class IRefSetter: public virtual ISetter<T,T&,T&,Derived> {
  public: 
    virtual ~IRefSetter() {}
};

template<typename T, class Derived =void> 
class IRefGetterSetter: 
  public virtual IGetterSetter<T,T&,T&,const T&,T&,Derived>,
  public virtual IRefGetter<T,Derived>,
  public virtual IRefSetter<T,Derived> 

{
  public: 
    virtual ~IRefGetterSetter() {}
};


template<typename T, class Derived =void> 
class IPtrGetter: public virtual IGetter<T,T&,T&,const T&,Derived> {
  public: 
    virtual ~IPtrGetter() {}
};

template<typename T, class Derived =void> 
class IPtrSetter: public virtual ISetter<T,T&,T&,Derived> {
  public: 
    virtual ~IPtrSetter() {}
};

template<typename T, class Derived =void> 
class IPtrGetterSetter: 
  public virtual IGetterSetter<T,T&,T&,const T&,T&,Derived>,
  public virtual IPtrGetter<T,Derived>,
  public virtual IPtrSetter<T,Derived> 

{
  public: 
    virtual ~IPtrGetterSetter() {}
};



template<typename T, class Derived =void> 
class IValGetter: public virtual IGetter<T,T&,T&,const T&,Derived> {
  public: 
    virtual ~IValGetter() {}
};

template<typename T, class Derived =void> 
class IValSetter: public virtual ISetter<T,T&,T&,Derived> {
  public: 
    virtual ~IValSetter() {}
};

template<typename T, class Derived =void> 
class IValGetterSetter: 
  public virtual IGetterSetter<T,T&,T&,const T&,T&,Derived>,
  public virtual IValGetter<T,Derived>,
  public virtual IValSetter<T,Derived> 

{
  public: 
    virtual ~IValGetterSetter() {}
};






template<typename T, typename MemberType, typename GetType, typename ConstGetType, typename SetType, class Derived =void> 
class GetterSetter: public virtual IGetterSetter<T,MemberType,GetType,ConstGetType,SetType, Derived> {
  protected:
    MemberType _member;

    GetterSetter(T& t): _member(t) {}
    GetterSetter(const T& t): _member(t) {}
#ifdef CPP11SUPPORT
    GetterSetter(T&& t): Base::_member(std::move(t)) {}
    template<bool B = std::is_trivially_constructible<T>::value, typename std::enable_if<B, int>::type = 0>
    GetterSetter(): Base::_member() {}
#endif

  public: 
    virtual ~GetterSetter() {}

    GetType  get(const TD<T>&) { return _member; };
    ConstGetType  get(const TD<T>&) const { return _member; };
    void set(const TD<T>&, SetType member) { _member = member; };
};

/*
 * 
 */


template<typename T, class Derived =void> 
class RefGetterSetter: 
  public virtual IRefGetterSetter<T,Derived>, 
  public GetterSetter<T,T&,T&,const T&,T&, Derived> {
    typedef GetterSetter<T,T&,T&,const T&,T&, Derived> Base;
  public: 
    RefGetterSetter(T& t): Base(t) {};
    //! No default constructor
#ifdef CPP11SUPPORT
    RefGetterSetter() = delete;
#endif
    virtual ~RefGetterSetter() {}
};

template<typename T, class Derived =void> 
class PtrGetterSetter: 
  public virtual IPtrGetterSetter<T, Derived>,
  public GetterSetter<T,T*,T*,T*,T*, Derived> {
    typedef GetterSetter<T,T*,T*,T*,T*,Derived> Base;
  public: 
    PtrGetterSetter(T* t): Base::_member(t) {}
    PtrGetterSetter(): Base::_member(NULL) {}
    virtual ~PtrGetterSetter() {}
};

template<typename T, class Derived =void> 
class ValGetterSetter: 
  public virtual IValGetterSetter<T, Derived>,
  public GetterSetter<T,T,T&, const T&,const T&,Derived> {
    typedef GetterSetter<T,T,T&, const T&,const T&, Derived> Base;
  public: 
    ValGetterSetter(const T& t): Base::_member(t) {}
#ifdef CPP11SUPPORT
    ValGetterSetter(T&& t): Base::_member(std::move(t)) {}
    template<bool B = std::is_trivially_constructible<T>::value, typename std::enable_if<B, int>::type = 0>
    ValGetterSetter(): Base::_member() {}
#else
    ValGetterSetter(): Base::_member() {}
#endif
    virtual ~ValGetterSetter() {}
};

// Forward declaration
template<typename T> class IExtension;

//! Define an Extension as RefGetterSetter
template<typename T>
class IExtensionGetter: public virtual IRefGetter<T, IExtension<T> > {
  public:
    virtual ~IExtensionGetter() {}
};
//! Define an Extension as RefGetterSetter
template<typename T>
class IExtension: 
  public virtual IExtensionGetter<T>,
  public virtual IRefGetterSetter<T, IExtension<T> > {
  public:
    using IRefGetterSetter<T, IExtension<T> >::get;
    virtual ~IExtension() {}
};

//! Define an Extension as RefGetterSetter
template<typename T>
class Extension: 
  public virtual IExtension<T>, 
  public RefGetterSetter<T,IExtension<T> > {
  public:
    typedef RefGetterSetter<T,IExtension<T> > Base;
    using RefGetterSetter<T,IExtension<T> >::get;
    using RefGetterSetter<T,IExtension<T> >::set;
    Extension(T& t): Base(t) {};
    //! No default constructor
#ifdef CPP11SUPPORT
    Extension() = delete;
#endif
    virtual ~Extension() {}
};


//! Define an ExtensionDelegate to Adopt existing extensions of extensions
//! \todo Generalize on GetterSetter?
template<typename T, typename D, typename Override=DummyBase<D> >
class IExtensionGetterDelegate: 
  public virtual Override,
  public virtual IExtensionGetter<T>,
  public virtual IExtensionGetter<D> {
  public:
    using IExtensionGetter<D>::get; 
    virtual ~IExtensionGetterDelegate() {}

    T&  get(const TD<T>& t) { return get(TD<D>()).get(t); };
    const T&  get(const TD<T>& t) const { return get(TD<D>()).get(t); };
};
//! Define an ExtensionDelegate to Adopt existing extensions of extensions
//! \todo Generalize on GetterSetter?
template<typename T, typename D, typename Override=DummyBase<D> >
class IExtensionDelegate: 
  public virtual Override,
  public virtual IExtensionGetterDelegate<T,D,Override>,
  public virtual IExtension<T> {
  public:
    using IExtensionGetterDelegate<T,D,Override>::get; 
    virtual ~IExtensionDelegate() {}

    void set(const TD<T>& t, T& s) { return get(TD<D>()).set(t,s); };
};


 
//! Define an ExtensionOverload to Adopt casting of extensions
//! \todo Generalize on GetterSetter?
template<typename T, typename D>
class IExtensionGetterOverload: 
    public virtual IExtensionGetter<D>,
    public virtual IExtensionGetter<T> {
  public:
    //using IExtensionGetter<D>::get; 
    using IExtensionGetter<T>::get; 
    virtual ~IExtensionGetterOverload() {}

    T&  get(const TD<T>&) { return IExtensionGetter<D>::get(TD<D>()); };
    const T&  get(const TD<T>&) const { return IExtensionGetter<D>::get(TD<D>()); };
};

//! Be carefull, intented to perform casts from base to derived classes
template<typename D, typename T>
class IExtensionGetterBaseCastOverload: 
    public virtual IExtensionGetter<T>,
    public virtual IExtensionGetter<D> {
  public:
    using IExtensionGetter<T>::get; 
    virtual ~IExtensionGetterBaseCastOverload() {}

    D&  get(const TD<D>&) { return dynamic_cast<D&>(get(TD<T>())); };
    const D&  get(const TD<D>&) const { return dynamic_cast<const D&>(get(TD<T>())); };
};
template<typename D, typename T>
class IExtensionBaseCastOverload: 
    public virtual IExtensionGetterBaseCastOverload<D,T>,
    protected virtual IExtension<T>,  // T setter is only used internally
    public virtual IExtension<D> {
  protected:
    using IExtension<T>::set;
  public:
    using IExtensionGetterBaseCastOverload<D,T>::get; 

    virtual ~IExtensionBaseCastOverload() {}
    void set(const TD<D>&, D& d) { set(TD<T>(), (T&)d); };
};

template<class CommonBase, typename IntegralTypeSpecifier>
class IDispatchDelegate: 
    public virtual IExtensionGetter<CommonBase> {
  public:

    virtual ~IDispatchDelegate() {}
    virtual const IntegralTypeSpecifier& getDispatchType() const =0;
};


template<class CommonBase, typename IntegralTypeSpecifier>
class DispatchDelegate: 
    public virtual IDispatchDelegate<CommonBase, IntegralTypeSpecifier>,
    public virtual IExtensionGetter<CommonBase>,
    protected Extension<CommonBase> {
  protected:
    IntegralTypeSpecifier _typeId;
  public:
    using Extension<CommonBase>::get;
    
    CommonBase&  get(const TD<CommonBase>& t) { return Extension<CommonBase>::get(t); };
    const CommonBase&  get(const TD<CommonBase>& t) const { return Extension<CommonBase>::get(t); };

    DispatchDelegate(CommonBase& t, const IntegralTypeSpecifier& typeId): 
      Extension<CommonBase>(t), _typeId(typeId) {}

    virtual ~DispatchDelegate() {}
    const IntegralTypeSpecifier& getDispatchType() const { return _typeId; };
};

template<class Derived, class CommonBase, typename IntegralTypeSpecifier>
class DispatchDelegateSpecialization: 
    public virtual DispatchDelegate<CommonBase, IntegralTypeSpecifier>,
    public virtual IExtensionBaseCastOverload<Derived,CommonBase> {
  protected:
    IntegralTypeSpecifier _typeId;
  public:
    using DispatchDelegate<CommonBase, IntegralTypeSpecifier>::get;
    using IExtensionBaseCastOverload<Derived,CommonBase>::get;

    DispatchDelegateSpecialization(Derived& d, const IntegralTypeSpecifier& typeId): 
      DispatchDelegate<CommonBase, IntegralTypeSpecifier>((CommonBase&)d, typeId) {}
    DispatchDelegateSpecialization(CommonBase& t, const IntegralTypeSpecifier& typeId): 
      DispatchDelegate<CommonBase, IntegralTypeSpecifier>(t, typeId) {}

    virtual ~DispatchDelegateSpecialization() {}
};

template<class CommonRefType, unsigned int Size>
class IDispatchDelegateHolder {
  public:
    virtual ~IDispatchDelegateHolder() {}

    virtual void* getDataPointer() =0;
    virtual void setConstructed()  =0;
    virtual CommonRefType* getDelegatePointer() =0;
};

template<class CommonRefType, unsigned int Size>
class DispatchDelegateHolder:
  public virtual IDispatchDelegateHolder<CommonRefType, Size> {
  protected:
    bool constructed;
    unsigned char mem[Size];
  public:
    DispatchDelegateHolder(): constructed(false) {}
    virtual ~DispatchDelegateHolder() {
      if(constructed) {
        getDelegatePointer()->~CommonRefType();
      }
    }

    void* getDataPointer() { return &mem[0]; };
    void setConstructed() { constructed = true; };
    bool isConstructed() const { return constructed; };

    CommonRefType* getDelegatePointer() { return (constructed ? ((CommonRefType*)getDataPointer()) : NULL); };
};







// #ifdef CPP14SUPPORT 
// #include <utility>
// 

// /*
//  * \todo Test, NOT USE YET
//  * 
//  * Future Proposol ones C++14 with perfect forwarding is standard
//  * 
//  * allows writing i.e.
//  * 
//  * class A:
//  *   public virtual HasGetterSetter<A>, // to provide general get/set mechanism
//  *   public virtual GetGetterSetter<Extension<...>>,
//  *   ...more Extensions...
//  *   {
//  *      using HasGetterSetter<A>::get;  // Select get, set explicitly if required
//  *      using HasGetterSetter<A>::set;  // Select get, set explicitly if required
//  * 
//  *      //! Explicitly load all getGetterSetter overloads
//  *      using GetGetterSetter<...>::getGetterSetter;
//  *      ...
//  *      using GetGetterSetter<...>::getGetterSetter;
//  * 
//  *      //! Benefit: for each Extension only getGetterSetter has to be declared here instead of get & set
//  *      //! for complex interfaces of this kinds with more than two functions a lot of redundant code is avoided
//  * 
//  *   }
//  */
// 

// template<typename GetterSetterClass>
// class IGetGetterSetter {
//   public: 
//     ~IGetGetterSetter() {}

//     virtual GetterSetterClass& getGetterSetter() =0;
//     virtual const GetterSetterClass& getGetterSetter() const =0;
// }

// template<typename GetterSetterClass>
// class GetGetterSetter: public virtual  IGetGetterSetter<GetterSetterClass> {
//   private:
//     GetterSetterClass _member;
//   public: 
//     template<typename T>
//     GetterSetterClass(T&& t): _member(std::forward<T>(t)) {}
//     template<bool B = std::is_trivially_constructible<T>::value, typename std::enable_if<B, int>::type = 0>
//     ValGetterSetter(): Base::_member() {}
//     ~GetGetterSetter() {}

//     virtual GetterSetterClass& getGetterSetter() { return _member; };
//     virtual const GetterSetterClass& getGetterSetter() const { return _member; };
// }
// 

// template<typename Derived>
// class HasGetterSetter {
//     template<typename T>
//     virtual decltype(auto)  get(const TD<T>& t) { return static_cast<Derived&>(*this).getGetterSetter(t).get(t); };
//     template<typename T>
//     virtual decltype(auto)  get(const TD<T>& t) const { return static_cast<const Derived&>(*this).getGetterSetter(t).get(t); };
//     template<typename T, typename SetType>
//     virtual void set(const TD<T>&, SetType&& sett) { static_cast<Derived&>(*this).getGetterSetter(t).set(t,std::forward<SetType>(sett)); };
// };

// #endif // CPP14SUPPORT




#endif /* ICommonUtils_HPP_ */
