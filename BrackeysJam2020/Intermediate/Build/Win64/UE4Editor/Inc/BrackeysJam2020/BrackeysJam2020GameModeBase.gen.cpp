// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BrackeysJam2020/BrackeysJam2020GameModeBase.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeBrackeysJam2020GameModeBase() {}
// Cross Module References
	BRACKEYSJAM2020_API UClass* Z_Construct_UClass_ABrackeysJam2020GameModeBase_NoRegister();
	BRACKEYSJAM2020_API UClass* Z_Construct_UClass_ABrackeysJam2020GameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_BrackeysJam2020();
// End Cross Module References
	void ABrackeysJam2020GameModeBase::StaticRegisterNativesABrackeysJam2020GameModeBase()
	{
	}
	UClass* Z_Construct_UClass_ABrackeysJam2020GameModeBase_NoRegister()
	{
		return ABrackeysJam2020GameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_BrackeysJam2020,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "BrackeysJam2020GameModeBase.h" },
		{ "ModuleRelativePath", "BrackeysJam2020GameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ABrackeysJam2020GameModeBase>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics::ClassParams = {
		&ABrackeysJam2020GameModeBase::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ABrackeysJam2020GameModeBase()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ABrackeysJam2020GameModeBase_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ABrackeysJam2020GameModeBase, 1259472382);
	template<> BRACKEYSJAM2020_API UClass* StaticClass<ABrackeysJam2020GameModeBase>()
	{
		return ABrackeysJam2020GameModeBase::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ABrackeysJam2020GameModeBase(Z_Construct_UClass_ABrackeysJam2020GameModeBase, &ABrackeysJam2020GameModeBase::StaticClass, TEXT("/Script/BrackeysJam2020"), TEXT("ABrackeysJam2020GameModeBase"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ABrackeysJam2020GameModeBase);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
