#if defined _isamp_items_included
	#endinput
#endif
#define _isamp_items_included

//==============================ITEM SYSTEM=====================================

#define ITEM_NONE               -1
#define ITEM_WEAPON             1
#define ITEM_OTHER              2
#define ITEM_STORABLE           3

//==============================DATA STORAGE====================================

enum ItemInfo {
	Name[40],
	ParamName[20],
	Type,
	bool:InvAvailable,
};

static const ServerItems[][ItemInfo] = {

/*0*/	{"NULO", "NULO", ITEM_NONE, false},
/*1*/	{"Manopla", "Cantidad", ITEM_WEAPON, true},
/*2*/	{"Palo de Golf", "Cantidad", ITEM_WEAPON, false},
/*3*/	{"Macana", "Cantidad", ITEM_WEAPON, true},
/*4*/	{"Navaja", "Cantidad", ITEM_WEAPON, true},
/*5*/	{"Bate", "Cantidad", ITEM_WEAPON, false},
/*6*/	{"Pala", "Cantidad", ITEM_WEAPON, false},
/*7*/	{"Palo de pool", "Cantidad", ITEM_WEAPON, false},
/*8*/	{"Katana", "Cantidad", ITEM_WEAPON, false},
/*9*/	{"Motosierra", "Cantidad", ITEM_WEAPON, false},
/*10*/	{"Consolador doble punta", "Cantidad", ITEM_WEAPON, true},
/*11*/	{"Consolador", "Cantidad", ITEM_WEAPON, true},
/*12*/	{"Vibrador", "Cantidad", ITEM_WEAPON, true},
/*13*/	{"Vibrador plateado", "Cantidad", ITEM_WEAPON, true},
/*14*/	{"Flores", "Cantidad", ITEM_WEAPON, true},
/*15*/	{"Baston", "Cantidad", ITEM_WEAPON, true},
/*16*/	{"Granada", "Cantidad", ITEM_WEAPON, true},
/*17*/	{"Granada de gas", "Cantidad", ITEM_WEAPON, true},
/*18*/	{"Bomba molotov", "Cantidad", ITEM_WEAPON, true},
/*19*/	{"NULO", "NULO", ITEM_NONE, false},
/*20*/	{"NULO", "NULO", ITEM_NONE, false},
/*21*/	{"NULO", "NULO", ITEM_NONE, false},
/*22*/	{"Pistola 9mm", "Munici�n", ITEM_WEAPON, true},
/*23*/	{"Pistola 9mm con silenciador", "Munici�n", ITEM_WEAPON, true},
/*24*/	{"Pistola Desert Eagle", "Munici�n", ITEM_WEAPON, true},
/*25*/	{"Escopeta", "Munici�n", ITEM_WEAPON, false},
/*26*/	{"Escopeta recortada", "Munici�n", ITEM_WEAPON, false},
/*27*/	{"Escopeta de combate", "Munici�n", ITEM_WEAPON, false},
/*28*/	{"Uzi", "Munici�n", ITEM_WEAPON, true},
/*29*/	{"Subfusil MP-5", "Munici�n", ITEM_WEAPON, false},
/*30*/	{"Fusil AK-47", "Munici�n", ITEM_WEAPON, false},
/*31*/	{"Fusil M4", "Munici�n", ITEM_WEAPON, false},
/*32*/	{"Pistola semiautomatica TEC-9", "Munici�n", ITEM_WEAPON, true},
/*33*/	{"Rifle de caza", "Munici�n", ITEM_WEAPON, false},
/*34*/	{"Rifle de francotirador", "Munici�n", ITEM_WEAPON, false},
/*35*/	{"RPG-7", "Munici�n", ITEM_WEAPON, false},
/*36*/	{"Lanzamisiles", "Munici�n", ITEM_WEAPON, false},
/*37*/	{"Lanzallamas", "Munici�n", ITEM_WEAPON, false},
/*38*/	{"Minigun", "Munici�n", ITEM_WEAPON, false},
/*39*/	{"Carga C4", "Cantidad", ITEM_WEAPON, false},
/*40*/	{"Detonador", "Cantidad", ITEM_WEAPON, true},
/*41*/	{"Aerosol", "Contenido", ITEM_WEAPON, true},
/*42*/	{"Extinguidor", "Contenido", ITEM_WEAPON, false},
/*43*/	{"C�mara fotogr�fica", "Fotos", ITEM_WEAPON, true},
/*44*/	{"Visi�n nocturna", "Cantidad", ITEM_WEAPON, false},
/*45*/	{"Visi�n t�rmica", "Cantidad", ITEM_WEAPON, false},
/*46*/	{"Paraca�das", "Cantidad", ITEM_WEAPON, false},
/*47*/	{"Caja de materiales", "Cantidad", ITEM_OTHER, false},
/*48*/	{"Bid�n de combustible", "Contenido", ITEM_OTHER, true},
/*49*/	{"Bolsa de materia prima", "Cantidad", ITEM_OTHER, false},
/*50*/	{"Productos", "Cantidad", ITEM_OTHER, false},
/*51*/	{"Dinero", "Cantidad", ITEM_OTHER, false},
/*52*/	{"Sandwich", "Cantidad", ITEM_OTHER, true},
/*53*/	{"Alfajor", "Cantidad", ITEM_OTHER, true},
/*54*/	{"Agua mineral", "Cantidad", ITEM_OTHER, true}

};

//==============================FUNCIONES=======================================

forward bool:HasItemInvPermission(itemid);
forward GetItemType(itemid);
forward GetItemName(itemid);
forward GetItemParamName(itemid);

//=======================IMPLEMENTACI�N DE FUNCIONES============================

stock bool:HasItemInvPermission(itemid)
{
	return ServerItems[itemid][InvAvailable];
}

stock GetItemType(itemid)
{
	return ServerItems[itemid][Type];
}

stock GetItemName(itemid)
{
	new str[40];
	format(str, sizeof(str), "%s" , ServerItems[itemid][Name]);
	return str;
}

stock GetItemParamName(itemid)
{
    new str[20];
    format(str, sizeof(str), "%s" , ServerItems[itemid][ParamName]);
	return str;
}