/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This is the default linker namespace for a vendor process (a process started
// from /vendor/bin/*).

#include "linkerconfig/namespacebuilder.h"

#include <android-base/strings.h>

#include "linkerconfig/common.h"
#include "linkerconfig/environment.h"

using android::linkerconfig::modules::Namespace;

namespace android {
namespace linkerconfig {
namespace contents {
Namespace BuildVendorDefaultNamespace(const Context& ctx) {
  return BuildVendorNamespace(ctx, "default");
}

Namespace BuildVendorNamespace([[maybe_unused]] const Context& ctx,
                               const std::string& name) {
  Namespace ns(name, /*is_isolated=*/true, /*is_visible=*/true);

  ns.AddSearchPath("/odm/${LIB}");
  ns.AddSearchPath("/vendor/${LIB}");
  ns.AddSearchPath("/vendor/${LIB}/hw");
  ns.AddSearchPath("/vendor/${LIB}/egl");

  ns.AddPermittedPath("/odm");
  ns.AddPermittedPath("/vendor");
  ns.AddPermittedPath("/system/vendor");

  if (ctx.IsVndkAvailable()) {
    ns.GetLink("rs").AddSharedLib("libRS_internal.so");
    ns.AddRequires(base::Split(Var("LLNDK_LIBRARIES_VENDOR", ""), ":"));
    ns.GetLink(ctx.GetSystemNamespaceName())
        .AddSharedLib(Var("SANITIZER_DEFAULT_VENDOR"));
    ns.GetLink("vndk").AddSharedLib({Var("VNDK_SAMEPROCESS_LIBRARIES_VENDOR"),
                                     Var("VNDK_CORE_LIBRARIES_VENDOR")});
    if (android::linkerconfig::modules::IsVndkInSystemNamespace()) {
      ns.GetLink("vndk_in_system")
          .AddSharedLib(Var("VNDK_USING_CORE_VARIANT_LIBRARIES"));
    }
  }

  ns.AddRequires(ctx.GetVendorRequireLibs());
  ns.AddProvides(ctx.GetVendorProvideLibs());
  return ns;
}
}  // namespace contents
}  // namespace linkerconfig
}  // namespace android
