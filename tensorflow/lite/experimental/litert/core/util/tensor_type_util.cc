// Copyright 2024 Google LLC.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tensorflow/lite/experimental/litert/core/util/tensor_type_util.h"

#include <cstddef>

#include "tensorflow/lite/experimental/litert/c/litert_common.h"
#include "tensorflow/lite/experimental/litert/c/litert_model.h"
#include "tensorflow/lite/experimental/litert/cc/litert_expected.h"

namespace litert {
namespace internal {

Expected<Ratio> GetElementSize(LiteRtElementType element_type) {
  switch (element_type) {
    case kLiteRtElementTypeInt4:
      return Ratio{1, 2};
    case kLiteRtElementTypeBool:
      return Ratio{1, 1};
    case kLiteRtElementTypeInt8:
    case kLiteRtElementTypeUInt8:
      return Ratio{1, 1};
    case kLiteRtElementTypeInt16:
    case kLiteRtElementTypeUInt16:
    case kLiteRtElementTypeFloat16:
    case kLiteRtElementTypeBFloat16:
      return Ratio{2, 1};
    case kLiteRtElementTypeInt32:
    case kLiteRtElementTypeUInt32:
    case kLiteRtElementTypeFloat32:
      return Ratio{4, 1};
    case kLiteRtElementTypeInt64:
    case kLiteRtElementTypeUInt64:
    case kLiteRtElementTypeFloat64:
      return Ratio{8, 1};
    case kLiteRtElementTypeComplex64:
      return Ratio{16, 1};
    case kLiteRtElementTypeComplex128:
      return Ratio{32, 1};
    default:
      return Unexpected(kLiteRtStatusErrorInvalidArgument,
                        "Unexpected element type");
  }
}

Expected<size_t> GetNumPackedBytes(const LiteRtRankedTensorType& type) {
  auto element_size = GetElementSize(type.element_type);
  if (!element_size) {
    return element_size.Error();
  }

  auto num_elements = GetNumElements(type);
  if (!num_elements) {
    return num_elements.Error();
  }

  return ((*num_elements * element_size->num) + (element_size->denom - 1)) /
         element_size->denom;
}

Expected<size_t> GetNumElements(const LiteRtRankedTensorType& tensor_type) {
  size_t num_elements = 1;
  for (auto i = 0; i < tensor_type.layout.rank; ++i) {
    auto dim = tensor_type.layout.dimensions[i];
    if (dim < 0) {
      return Unexpected(kLiteRtStatusErrorInvalidArgument,
                        "Unexpected dynamic tensor passed as input");
    } else if (dim == 0) {
      return Unexpected(kLiteRtStatusErrorInvalidArgument,
                        "Unexpected 0 tensor dimension");
    }
    num_elements *= dim;
  }

  return num_elements;
}

}  // namespace internal
}  // namespace litert
