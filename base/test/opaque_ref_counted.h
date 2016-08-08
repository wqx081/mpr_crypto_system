#ifndef BASE_TEST_OPAQUE_REF_COUNTED_H_
#define BASE_TEST_OPAQUE_REF_COUNTED_H_
#include "base/ref_counted.h"

namespace base {

class OpaqueRefCounted;

scoped_ref_ptr<OpaqueRefCounted> MakeOpaqueRefCounted();
void TestOpaqueRefCounted(scoped_ref_ptr<OpaqueRefCounted> p);

} // namespace base

extern template class scoped_ref_ptr<base::OpaqueRefCounted>;

#endif // BASE_TEST_OPAQUE_REF_COUNTED_H_
