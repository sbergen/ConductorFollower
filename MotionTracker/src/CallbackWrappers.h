#include <XnCppWrapper.h>

namespace cf {
namespace MotionTracker {

template<typename T1, typename T2, typename T3, typename ObjType, void (ObjType::*CallbackFnPtr)(T1, T2, T3)>
void XN_CALLBACK_TYPE CallbackWrapper(T1 a1, T2 a2, T3 a3, void * obj)
{
	(static_cast<ObjType *>(obj)->*CallbackFnPtr)(a1, a2, a3);
}

template<typename T1, typename T2, typename T3, typename T4, typename ObjType, void (ObjType::*CallbackFnPtr)(T1, T2, T3, T4)>
void XN_CALLBACK_TYPE CallbackWrapper(T1 a1, T2 a2, T3 a3, T4 a4, void * obj)
{
	(static_cast<ObjType *>(obj)->*CallbackFnPtr)(a1, a2, a3, a4);
}

} // namespace MotionTacker
} // namespace cf
