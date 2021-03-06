#include "PipelineState.hpp"

namespace inl {
namespace gxapi_dx12 {

PipelineState::PipelineState(ComPtr<ID3D12PipelineState> native)
	: m_native{native}
{}

ID3D12PipelineState* PipelineState::GetNative() {
	return m_native.Get();
}


} // namespace gxapi_dx12
} // namespace inl
