#pragma once
#include "UE4.hpp"

namespace Unreal {
	struct UField_Old : UObject
	{
		UField_Old* Next;
	};

	struct UField_New : UObject
	{
		UField_New* Next;
		void* UKD_0;
		void* UKD_1;
	};

	struct UStruct_Old : UField_Old
	{
		UStruct_Old* Super;
		UField_Old* Children;
		uint32_t Size;
		char PAD[0x44];
	};

	struct UStruct_New : UField_New
	{
		UStruct_New* Super;
		UField_New* Children;
		uint32_t Size;
		char PAD[0x44];
	};

	struct FField {
		void* VTable;
		void* Class;
		void* Owner;
		void* UKD_00;
		FField* Next;
		FName Name;
		int32_t FlagsPrivate;
	};

	struct FProperty : FField
	{
		int32_t ArrayDim;
		int32_t ElementSize;
		int32_t PropertyFlags;
		uint16_t RepIndex;
		void* BlueprintReplicationCondition;
		int32_t Offset_Internal;
		FName RepNotifyFunc;
		FProperty* PropertyLinkNext;
		FProperty* NextRef;
		FProperty* DestructorLinkNext;
		FProperty* PostConstructLinkNext;
	};


	struct UStruct_New2 : UField_New
	{
		UStruct_New2* Super;
		UField_New* Children;
		FField* ChildProperties;
		int32_t Size;
		int32_t MinAlignment;
		TArray<uint8_t> Script;
		FProperty* PropertyLink;
		FProperty* RefLink;
		FProperty* DestructorLink;
		FProperty* PostConstructLink;
	};
}

namespace Finder {
	using namespace Unreal;
	UObject* FindChild(Unreal::UObject* InObject, std::string PropName) {
		if (GVersion < 5.0f) {
			UField_Old* Prop = reinterpret_cast<UStruct_Old*>(InObject)->Children;
			while (Prop)
			{
				if (Prop->Name.GetName() == PropName) {
					return Prop;
				}
				else {
					Prop = Prop->Next;
				}
			}
			return nullptr;
		}
		else if (GVersion < 12.0f) {
			UField_New* Prop = reinterpret_cast<UStruct_New*>(InObject)->Children;
			while (Prop)
			{
				if (Prop->Name.GetName() == PropName) {
					return Prop;
				}
				else {
					Prop = Prop->Next;
				}
			}
			return nullptr;
		}
		else {
			FField* Prop = reinterpret_cast<UStruct_New2*>(InObject)->ChildProperties;
			while (Prop)
			{
				if (Prop->Name.GetName() == PropName) {
					return (UObject*)Prop;
				}
				else {
					Prop = Prop->Next;
				}
			}
			return nullptr;
		}
	}

	int GetOffset(void* TargetProp) {
		if (GVersion < 12.0f) {
			return *reinterpret_cast<int*>(__int64(TargetProp) + 0x44);
		}
		else {
			return *reinterpret_cast<int*>(__int64(TargetProp) + 0x4C);
		}
	}

	template<class T>
	T GetValuePointer(UObject* Object, void* Prop) {
		return reinterpret_cast<T>(Object->GetAddress() + GetOffset(Prop));
	}

	int GetPropByClass(Unreal::UObject* TargetClass, std::string TargetChildName) {
		UObject* Prop = nullptr;
		if (GVersion < 5.0f) {
			UStruct_Old* Class = (UStruct_Old*)TargetClass;
			if (Class->Children) {
				Prop = FindChild(Class, TargetChildName);
			}
			if (Prop == nullptr) {
				UStruct_Old* Struct = reinterpret_cast<UStruct_Old*>(Class)->Super;
				while (Struct)
				{
					if (Struct->Children) {
						Prop = FindChild(Struct, TargetChildName);
						if (Prop != nullptr) {
							break;
						}
					}
					Struct = Struct->Super;
				}
			}
		}
		else if (GVersion < 12.0f) {
			UStruct_New* Class = (UStruct_New*)TargetClass;
			if (Class->Children) {
				Prop = FindChild(Class, TargetChildName);
			}
			if (Prop == nullptr) {
				UStruct_New* Struct = reinterpret_cast<UStruct_New*>(Class)->Super;
				while (Struct)
				{
					if (Struct->Children) {
						Prop = FindChild(Struct, TargetChildName);
						if (Prop != nullptr) {
							break;
						}
					}
					Struct = Struct->Super;
				}
			}
		}
		else {
			UStruct_New2* Class = (UStruct_New2*)TargetClass;
			if (Class->ChildProperties) {
				Prop = FindChild(Class, TargetChildName);
			}
			if (Prop == nullptr) {
				UStruct_New2* Struct = reinterpret_cast<UStruct_New2*>(Class)->Super;
				while (Struct)
				{
					if (Struct->ChildProperties) {
						Prop = FindChild(Struct, TargetChildName);
						if (Prop != nullptr) {
							break;
						}
					}
					Struct = Struct->Super;
				}
			}
		}
		return GetOffset(Prop);
	}

	//Doesnt work on structs only UObject Based Types and types like ints, bools, etc (i think)
	template<class T = Unreal::UObject**>
	T Find(UObject* TargetObject, std::string TargetChildName) {
		UObject* Prop = nullptr;
		if (GVersion < 5.0f) {
			UStruct_Old* Class = (UStruct_Old*)TargetObject->Class;
			if (Class->Children) {
				Prop = FindChild(Class, TargetChildName);
			}
			if (Prop == nullptr) {
				UStruct_Old* Struct = reinterpret_cast<UStruct_Old*>(TargetObject->Class)->Super;
				while (Struct)
				{
					if (Struct->Children) {
						Prop = FindChild(Struct, TargetChildName);
						if (Prop != nullptr) {
							break;
						}
					}
					Struct = Struct->Super;
				}
			}
		}
		else if (GVersion < 12.0f) {
			UStruct_New* Class = (UStruct_New*)TargetObject->Class;
			if (Class->Children) {
				Prop = FindChild(Class, TargetChildName);
			}
			if (Prop == nullptr) {
				UStruct_New* Struct = reinterpret_cast<UStruct_New*>(TargetObject->Class)->Super;
				while (Struct)
				{
					if (Struct->Children) {
						Prop = FindChild(Struct, TargetChildName);
						if (Prop != nullptr) {
							break;
						}
					}
					Struct = Struct->Super;
				}
			}
		}
		else {
			UStruct_New2* Class = (UStruct_New2*)TargetObject->Class;
			if (Class->ChildProperties) {
				Prop = FindChild(Class, TargetChildName);
			}
			if (Prop == nullptr) {
				UStruct_New2* Struct = reinterpret_cast<UStruct_New2*>(TargetObject->Class)->Super;
				while (Struct)
				{
					if (Struct->ChildProperties) {
						Prop = FindChild(Struct, TargetChildName);
						if (Prop != nullptr) {
							break;
						}
					}
					Struct = Struct->Super;
				}
			}
		}
		if (Prop != nullptr) {
			return GetValuePointer<T>(TargetObject, Prop);
		}
		else {
			return nullptr;
		}
	}

	UObject* FindActor(UObject* Class, int Index = 0) {
		struct {
			UObject* World;
			UObject* Class;
			TArray<UObject*> Actors;
		} Params;
		Params.World = Game::GWorld;
		Params.Class = Class;
		FindObject("/Script/Engine.Default__GameplayStatics")->ProcessEvent(FindObject("/Script/Engine.GameplayStatics:GetAllActorsOfClass"), &Params);
		auto Actors = Params.Actors;
		return Actors.Data[Index];
	}

	inline UObject* SpawnActor(UObject* StaticClass, FVector Location = { 0.0f, 0.0f, 0.0f }, UObject* Owner = nullptr, FQuat Rotation = { 0, 0, 0 }, ESpawnActorCollisionHandlingMethod Flags = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		FTransform SpawnTransform;

		SpawnTransform.Translation = Location;
		SpawnTransform.Scale3D = FVector{ 1, 1, 1 };
		SpawnTransform.Rotation = Rotation;

		UGameplayStatics_BeginDeferredActorSpawnFromClass_Params params;
		params.WorldContextObject = Game::GWorld;
		params.ActorClass = StaticClass;
		params.SpawnTransform = SpawnTransform;
		params.CollisionHandlingOverride = Flags;
		params.Owner = Owner;

		FindObject("Default__GameplayStatics")->ProcessEvent(FindObject("BeginDeferredActorSpawnFromClass"), &params);
		UObject* FirstActor = params.ReturnValue;

		if (FirstActor)
		{
			UGameplayStatics_FinishSpawningActor_Params params;
			params.Actor = FirstActor;
			params.SpawnTransform = SpawnTransform;
			FindObject("Default__GameplayStatics")->ProcessEvent(FindObject("FinishSpawningActor"), &params);
			
			if (FirstActor)
				return FirstActor;
		}

		return nullptr;
	}

	template <typename ActorType>
	inline ActorType* SpawnActor(FVector Location = { 0.0f, 0.0f, 0.0f }, UObject* Owner = nullptr, FQuat Rotation = { 0, 0, 0 }, ESpawnActorCollisionHandlingMethod Flags = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		return (ActorType*)SpawnActor(ActorType::StaticClass(), Location, Owner, Rotation, Flags);
	}
}