// Copyright 1998-2023 Epic Games, Inc. All Rights Reserved.
// =================================================
// Created by: Galidar
// Project name: Oceanology
// Created on: 2023/12/15
//
// =================================================


#include "Components/QuadTree/OceanologyWaterVertexFactory.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "MeshDrawShaderBindings.h"
#include "MeshBatch.h"
#include "MeshMaterialShader.h"
#include "RenderUtils.h"

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FOceanologyWaterVertexFactoryParameters, "OceanologyWaterVF");
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FOceanologyWaterVertexFactoryRaytracingParameters,
                                         "OceanologyWaterRaytracingVF");

/**
 * Shader parameters for water vertex factory.
 */
template <bool bWithWaterSelectionSupport>
class TOceanologyWaterVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
	DECLARE_TYPE_LAYOUT(TOceanologyWaterVertexFactoryShaderParameters<bWithWaterSelectionSupport>, NonVirtual);

public:
	using OceanologyWaterVertexFactoryType = TOceanologyWaterVertexFactory<bWithWaterSelectionSupport>;
	using OceanologyWaterMeshUserDataType = TOceanologyWaterMeshUserData<bWithWaterSelectionSupport>;
	using OceanologyWaterInstanceDataBuffersType = TOceanologyWaterInstanceDataBuffers<bWithWaterSelectionSupport>;

	void Bind(const FShaderParameterMap& ParameterMap)
	{
	}

	void GetElementShaderBindings(
		const class FSceneInterface* Scene,
		const class FSceneView* View,
		const class FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const class FVertexFactory* InVertexFactory,
		const struct FMeshBatchElement& BatchElement,
		class FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams) const
	{
		OceanologyWaterVertexFactoryType* VertexFactory = (OceanologyWaterVertexFactoryType*)InVertexFactory;

		const OceanologyWaterMeshUserDataType* WaterMeshUserData = (const OceanologyWaterMeshUserDataType*)BatchElement.
			UserData;

		const OceanologyWaterInstanceDataBuffersType* InstanceDataBuffers = WaterMeshUserData->InstanceDataBuffers;

		const int32 InstanceOffsetValue = BatchElement.UserIndex;

		ShaderBindings.Add(Shader->GetUniformBufferParameter<FOceanologyWaterVertexFactoryParameters>(),
		                   VertexFactory->GeTOceanologyWaterVertexFactoryUniformBuffer(
			                   WaterMeshUserData->RenderGroupType));

#if RHI_RAYTRACING
		if (IsRayTracingEnabled())
		{
			ShaderBindings.Add(Shader->GetUniformBufferParameter<FOceanologyWaterVertexFactoryRaytracingParameters>(),
			                   WaterMeshUserData->WaterVertexFactoryRaytracingVFUniformBuffer);
		}
#endif

		if (VertexStreams.Num() > 0)
		{
			for (int32 i = 0; i < OceanologyWaterInstanceDataBuffersType::NumBuffers; ++i)
			{
				FVertexInputStream* InstanceInputStream = VertexStreams.FindByPredicate(
					[i](const FVertexInputStream& InStream) { return InStream.StreamIndex == i + 1; });
				check(InstanceInputStream);

				// Bind vertex buffer
				check(InstanceDataBuffers->GetBuffer(i));
				InstanceInputStream->VertexBuffer = InstanceDataBuffers->GetBuffer(i);
			}

			if (InstanceOffsetValue > 0)
			{
				VertexFactory->OffsetInstanceStreams(InstanceOffsetValue, InputStreamType, VertexStreams);
			}
		}
	}
};

// ----------------------------------------------------------------------------------

// Always implement the basic vertex factory so that it's there for both editor and non-editor builds :
IMPLEMENT_TEMPLATE_TYPE_LAYOUT(
	template<>, TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */ false>);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ false>,
                                        SF_Vertex,
                                        TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */
                                        false>);
#if RHI_RAYTRACING
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ false>,
                                        SF_Compute,
                                        TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */
                                        false>);

IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ false>,
                                        SF_RayHitGroup,
                                        TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */
                                        false>);
#endif // RHI_RAYTRACING
IMPLEMENT_TEMPLATE_VERTEX_FACTORY_TYPE(
	template<>, TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ false>,
	"/Plugin/Oceanology_Plugin/Private/OceanologyWaterMeshVertexFactory.ush",
	EVertexFactoryFlags::UsedWithMaterials
	| EVertexFactoryFlags::SupportsDynamicLighting
	| EVertexFactoryFlags::SupportsPrecisePrevWorldPos
	| EVertexFactoryFlags::SupportsPrimitiveIdStream
	| EVertexFactoryFlags::SupportsRayTracing
	| EVertexFactoryFlags::SupportsRayTracingDynamicGeometry
	| EVertexFactoryFlags::SupportsPSOPrecaching
);

#if WITH_WATER_SELECTION_SUPPORT

// In editor builds, also implement the vertex factory that supports water selection:
IMPLEMENT_TEMPLATE_TYPE_LAYOUT(
	template<>, TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */ true>);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ true>,
                                        SF_Vertex,
                                        TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */
                                        true>);
#if RHI_RAYTRACING
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ true>,
                                        SF_Compute,
                                        TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */
                                        true>);

IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ true>,
                                        SF_RayHitGroup,
                                        TOceanologyWaterVertexFactoryShaderParameters</*bWithWaterSelectionSupport = */
                                        true>);
#endif
IMPLEMENT_TEMPLATE_VERTEX_FACTORY_TYPE(
	template<>, TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ true>,
	"/Plugin/Oceanology_Plugin/Private/OceanologyWaterMeshVertexFactory.ush",
	EVertexFactoryFlags::UsedWithMaterials
	| EVertexFactoryFlags::SupportsDynamicLighting
	| EVertexFactoryFlags::SupportsPrecisePrevWorldPos
	| EVertexFactoryFlags::SupportsPrimitiveIdStream
	| EVertexFactoryFlags::SupportsRayTracing
	| EVertexFactoryFlags::SupportsRayTracingDynamicGeometry
	| EVertexFactoryFlags::SupportsPSOPrecaching
);

#endif // WITH_WATER_SELECTION_SUPPORT

const FVertexFactoryType* GetOceanologyWaterVertexFactoryType(bool bWithWaterSelectionSupport)
{
#if WITH_WATER_SELECTION_SUPPORT
	if (bWithWaterSelectionSupport)
	{
		return &TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ true>::StaticType;
	}
	else
#endif // WITH_WATER_SELECTION_SUPPORT
	{
		check(!bWithWaterSelectionSupport);
		return &TOceanologyWaterVertexFactory</*bWithWaterSelectionSupport = */ false>::StaticType;
	}
}
