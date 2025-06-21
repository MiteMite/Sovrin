// 


#include "InventoryComponent.h"
#include "GameFramework/Actor.h"


UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    InventorySlots.Reserve(MaxInventorySize);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    InventorySlots.SetNum(MaxInventorySize);
}

bool UInventoryComponent::AddItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity)
{
    if (!Equipment.GetInterface() || Quantity <= 0)
    {
        return false;
    }

    if (!CanAddItem(Equipment, Quantity))
    {
        return false;
    }

    // Try to find existing slot with same item
    int32 ExistingSlot = FindItemSlot(Equipment);
    if (ExistingSlot != INDEX_NONE)
    {
        InventorySlots[ExistingSlot].Quantity += Quantity;
        return true;
    }

    // Find empty slot
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (!InventorySlots[i].Equipment.GetInterface())
        {
            InventorySlots[i].Equipment = Equipment;
            InventorySlots[i].Quantity = Quantity;
            InventorySlots[i].bIsEquipped = false;
            return true;
        }
    }

    return false;
}

bool UInventoryComponent::RemoveItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity)
{
    if (!Equipment.GetInterface() || Quantity <= 0)
    {
        return false;
    }

    int32 SlotIndex = FindItemSlot(Equipment);
    if (SlotIndex == INDEX_NONE)
    {
        return false;
    }

    FInventorySlot& Slot = InventorySlots[SlotIndex];
    if (Slot.Quantity < Quantity)
    {
        return false;
    }

    Slot.Quantity -= Quantity;
    if (Slot.Quantity <= 0)
    {
        // If item is equipped, unequip it first
        if (Slot.bIsEquipped)
        {
            UnequipItem(Equipment.GetInterface()->GetEquipmentSlot());
        }
        
        Slot.Equipment = nullptr;
        Slot.Quantity = 0;
        Slot.bIsEquipped = false;
    }

    return true;
}

bool UInventoryComponent::HasItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity) const
{
    int32 SlotIndex = FindItemSlot(Equipment);
    if (SlotIndex == INDEX_NONE)
    {
        return false;
    }

    return InventorySlots[SlotIndex].Quantity >= Quantity;
}

bool UInventoryComponent::EquipItem(TScriptInterface<IEquipmentInterface> Equipment)
{
    if (!Equipment.GetInterface())
    {
        return false;
    }

    // Check if item is in inventory
    int32 SlotIndex = FindItemSlot(Equipment);
    if (SlotIndex == INDEX_NONE)
    {
        return false;
    }

    // Check if character can equip this item
    if (!Equipment.GetInterface()->CanEquip(GetOwner()))
    {
        return false;
    }

    EEquipmentSlot TargetSlot = Equipment.GetInterface()->GetEquipmentSlot();
    
    // Unequip current item in this slot if any
    if (EquippedItems.Contains(TargetSlot))
    {
        UnequipItem(TargetSlot);
    }

    // Equip the new item
    if (Equipment.GetInterface()->Equip(GetOwner()))
    {
        EquippedItems.Add(TargetSlot, Equipment);
        InventorySlots[SlotIndex].bIsEquipped = true;
        Equipment.GetInterface()->OnEquipped(GetOwner());
        return true;
    }

    return false;
}

bool UInventoryComponent::UnequipItem(EEquipmentSlot Slot)
{
    if (!EquippedItems.Contains(Slot))
    {
        return false;
    }

    TScriptInterface<IEquipmentInterface> Equipment = EquippedItems[Slot];
    if (!Equipment.GetInterface())
    {
        return false;
    }

    if (Equipment.GetInterface()->Unequip(GetOwner()))
    {
        // Find the item in inventory and mark as unequipped
        int32 SlotIndex = FindItemSlot(Equipment);
        if (SlotIndex != INDEX_NONE)
        {
            InventorySlots[SlotIndex].bIsEquipped = false;
        }

        Equipment.GetInterface()->OnUnequipped(GetOwner());
        EquippedItems.Remove(Slot);
        return true;
    }

    return false;
}

TScriptInterface<IEquipmentInterface> UInventoryComponent::GetEquippedItem(EEquipmentSlot Slot) const
{
    if (EquippedItems.Contains(Slot))
    {
        return EquippedItems[Slot];
    }
    return nullptr;
}

TArray<TScriptInterface<IEquipmentInterface>> UInventoryComponent::GetAllEquippedItems() const
{
    TArray<TScriptInterface<IEquipmentInterface>> Result;
    EquippedItems.GenerateValueArray(Result);
    return Result;
}

int32 UInventoryComponent::GetAvailableSlots() const
{
    int32 UsedSlots = 0;
    for (const FInventorySlot& Slot : InventorySlots)
    {
        if (Slot.Equipment.GetInterface())
        {
            UsedSlots++;
        }
    }
    return MaxInventorySize - UsedSlots;
}

TArray<FInventorySlot> UInventoryComponent::GetItemsByType(EEquipmentType Type) const
{
    TArray<FInventorySlot> Result;
    for (const FInventorySlot& Slot : InventorySlots)
    {
        if (Slot.Equipment.GetInterface() && Slot.Equipment.GetInterface()->GetEquipmentType() == Type)
        {
            Result.Add(Slot);
        }
    }
    return Result;
}

TArray<FInventorySlot> UInventoryComponent::GetEquippableItems(EEquipmentSlot Slot) const
{
    TArray<FInventorySlot> Result;
    for (const FInventorySlot& InventorySlot : InventorySlots)
    {
        if (InventorySlot.Equipment.GetInterface() && 
            InventorySlot.Equipment.GetInterface()->GetEquipmentSlot() == Slot &&
            !InventorySlot.bIsEquipped)
        {
            Result.Add(InventorySlot);
        }
    }
    return Result;
}

int32 UInventoryComponent::FindItemSlot(TScriptInterface<IEquipmentInterface> Equipment) const
{
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (InventorySlots[i].Equipment.GetInterface() == Equipment.GetInterface())
        {
            return i;
        }
    }
    return INDEX_NONE;
}

bool UInventoryComponent::CanAddItem(TScriptInterface<IEquipmentInterface> Equipment, int32 Quantity) const
{
    // Check if we have space
    if (GetAvailableSlots() <= 0 && FindItemSlot(Equipment) == INDEX_NONE)
    {
        return false;
    }

    return true;
}




