#if defined _marp_container_included
	#endinput
#endif
#define _marp_container_included

////////////////////////////////////////////////////////////////////////////////
/*
		ESTRUCTURA DE UN CONTENEDOR USANDO VECTORES DINAMICOS
	 	 _______________________________________________________
	   	|    |    |    |    |    |    |    |    |    |    |    |
		| 0  | 1  | 2  | 3  | 4  | 5  |  6 | 7  | 8  | 9  | ...|
	    |    |    |    |    |    |    |    |    |    |    |    |
     	|____|____|____|____|____|____|____|____|____|____|____|

      0: SQLID del contenedor
      1: M�nimo espacio de item aceptado
      2: Espacio total del contenedor
      3: Espacio usado del contenedor
      4: Item1
      5: Param1
      6: Item2
      7: Param2
      8: Item3
      9: Param3
      ...
*/
////////////////////////////////////////////////////////////////////////////////

#define CONTAINER_TYPE_TRUNK     		1
#define CONTAINER_TYPE_INV  	   		2
#define CONTAINER_TYPE_HOUSE      		3
#define CONTAINER_TYPE_ITEM        		4

#define CONTAINER_INDEX_SQLID			0
#define CONTAINER_INDEX_MIN_ITEM_SPACE  1
#define CONTAINER_INDEX_TOTAL_SPACE 	2
#define CONTAINER_INDEX_USED_SPACE   	3

enum ContainerSelectionInfo {
	csId,
	csType,
	csOriginalId
}

new Container_Selection[MAX_PLAYERS][ContainerSelectionInfo];

new Container_DlgInfoStr[4096];

//==============================================================================

ResetContainerSelection(playerid)
{
   	Container_Selection[playerid][csId] = 0;
	Container_Selection[playerid][csType] = 0;
	Container_Selection[playerid][csOriginalId] = 0;
}

//==============================================================================

stock Container_Load(container_sqlid)
{
	new query[128];
	new container_id = vector_create();

	vector_push_back(container_id, container_sqlid); // Guarda la sqlid de si mismo.
	vector_push_back(container_id, 0); // Inicializamos donde se va a guardar el espacio m�nimo por item.
	vector_push_back(container_id, 0); // Inicializamos donde se va a guardar el espacio total del contenedor.
	vector_push_back(container_id, 0); // Inicializamos donde se va a guardar el espacio usado del contenedor.

 	format(query, sizeof(query), "SELECT * FROM containers_info WHERE container = %d LIMIT 1", container_sqlid);
	mysql_function_query(dbHandle, query, true, "OnContainerDataLoad", "i", container_id);

 	format(query, sizeof(query), "SELECT * FROM containers_slots WHERE container = %d", container_sqlid);
	mysql_function_query(dbHandle, query, true, "OnContainerSlotsLoad", "i", container_id);

	return container_id;
}

//==============================================================================

forward OnContainerDataLoad(container_id);
public OnContainerDataLoad(container_id)
{
	new rows,
		fields;

	cache_get_data(rows, fields);

	if(rows)
	{
		vector_set(container_id, CONTAINER_INDEX_MIN_ITEM_SPACE, cache_get_field_content_int(0, "MinItemSpace"));
		vector_set(container_id, CONTAINER_INDEX_TOTAL_SPACE, cache_get_field_content_int(0, "TotalSpace"));
	}
	return 1;
}

//==============================================================================

forward OnContainerSlotsLoad(container_id);
public OnContainerSlotsLoad(container_id)
{
	new rows,
		fields,
		item_id,
		param;

	cache_get_data(rows, fields);

	if(rows)
	{
	    for(new i = 0; i < rows; i++)
	    {
			item_id = cache_get_field_content_int(i, "Item");
        	vector_push_back(container_id, item_id);
        	
        	vector_set(container_id, CONTAINER_INDEX_USED_SPACE, vector_get(container_id, CONTAINER_INDEX_USED_SPACE) + GetItemSpace(item_id));
        	
        	param = cache_get_field_content_int(i, "Param");
        	
			if(GetItemType(item_id) == ITEM_CONTAINER)
			{
				vector_push_back(container_id, Container_Load(param));
			}
			else
			{
				vector_push_back(container_id, param);
			}
		}
	}
	return 1;
}

//==============================================================================

stock Container_Create(total_space, min_item_space, &container_id, &container_sqlid)
{
	if(total_space >= 0) // Espacio v�lido
	{
        new sqlid_address = GetVariableAddress(container_sqlid); // Guarda la direcci�n de memoria de la variable donde hay que guardar la SQLID creada con la query INSERT.

        container_id = vector_create();

  		vector_push_back(container_id, 0); // Reservado para su SQLID
		vector_push_back(container_id, min_item_space); // Minimo espacio por item que permite
		vector_push_back(container_id, total_space); // Espacio total
		vector_push_back(container_id, 0); // Espacio usado

		new query[128];
		
		format(query, sizeof(query), "INSERT INTO containers_info (MinItemSpace, TotalSpace) VALUES (%d, %d)", min_item_space, total_space);
		mysql_function_query(dbHandle, query, true, "GetContainerInsertedId", "dd", container_id, sqlid_address);
		return 1;
	}

	return 0;
}

forward GetContainerInsertedId(container_id, address);
public GetContainerInsertedId(container_id, address)
{
	new last_id = cache_insert_id(dbHandle);
    @ptr[address] = last_id;
    vector_set(container_id, CONTAINER_INDEX_SQLID, last_id);
    return 1;
}

//==============================================================================

stock Container_Fully_Destroy(id, container_sqlid)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
	    new query[128];
		new vec_size = vector_size(id);

		for(new i = 4; i < vec_size; i += 2) // Para todos los items almacenados en el vector que simula el contenedor
		{
		    if(GetItemType(vector_get(id, i)) == ITEM_CONTAINER) // Como "i" cae en los items, obtenemos el valor en posicion "i" y nos fijamos
			{
			    Container_Fully_Destroy(vector_get(id, i+1), Container_GetSQLID(vector_get(id, i+1))); // Los items CONTAINER, en su parametro guardan la id del vector real de ejecucion de su contenedor
			}
		}
		
	    format(query, sizeof(query), "DELETE FROM containers_info WHERE container = %d", container_sqlid);
	    mysql_function_query(dbHandle, query, false, "", "");

	    format(query, sizeof(query), "DELETE FROM containers_slots WHERE container = %d", container_sqlid);
	    mysql_function_query(dbHandle, query, false, "", "");

		vector_destroy(id);
		
		return 1;
	}

	return 0;
}

//==============================================================================

stock Container_Destroy(id)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
	    new vec_size = vector_size(id);
	    
		for(new i = 4; i < vec_size; i += 2) // Para todos los items almacenados en el vector que simula el contenedor
		{
		    if(GetItemType(vector_get(id, i)) == ITEM_CONTAINER) // Como "i" cae en los items, obtenemos el valor en posicion "i" y nos fijamos
			{
			    Container_Destroy(vector_get(id, i+1)); // Los items CONTAINER, en su parametro guardan la id del vector real de ejecucion de su contenedor
			}
		}
		vector_destroy(id);
		return 1;
	}

	return 0;
}

//==============================================================================

stock Container_AddUsedSpace(id, space)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
		vector_set(id, CONTAINER_INDEX_USED_SPACE, vector_get(id, CONTAINER_INDEX_USED_SPACE) + space);
		return 1;
	}

	return 0;
}

//==============================================================================

stock Container_GetTotalSpace(id)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
     return vector_get(id, CONTAINER_INDEX_TOTAL_SPACE);

	return 0;
}

//==============================================================================

stock Container_SetTotalSpace(id, total_space)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
	    vector_set(id, CONTAINER_INDEX_TOTAL_SPACE, total_space);

		new query[128];

		format(query, sizeof(query), "UPDATE containers_info SET TotalSpace = %d WHERE container = %d", total_space, Container_GetSQLID(id));
		mysql_function_query(dbHandle, query, false, "", "");

		return 1;
	}
	
	return 0;
}

//==============================================================================

stock Container_GetUsedSpace(id)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	    return vector_get(id, CONTAINER_INDEX_USED_SPACE);

	return 99999;
}

//==============================================================================

stock Container_GetFreeSpace(id)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
		return vector_get(id, CONTAINER_INDEX_TOTAL_SPACE) - vector_get(id, CONTAINER_INDEX_USED_SPACE);
	}
	return 0;
}

//==============================================================================

stock Container_GetElementsAmount(id)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	    return (vector_size(id) - 4) / 2;

	return 0;
}

//==============================================================================

stock Container_GetSQLID(id)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	    return vector_get(id, CONTAINER_INDEX_SQLID);

	return 0;
}

//==============================================================================

stock Container_GetMinItemSpace(id)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	    return vector_get(id, CONTAINER_INDEX_MIN_ITEM_SPACE);

	return 0;
}

//==============================================================================

stock Container_SlotToVecIndex(container_slot)
{
	return (container_slot + 2) * 2;
}

//==============================================================================

stock Container_VecIndexToSlot(vec_index)
{
	return (vec_index - 4) / 2;
}

//==============================================================================

stock Container_GetItem(id, slot)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
		if(slot < 0 || slot >= Container_GetElementsAmount(id)) // Slot inv�lido
	        return 0;

		return vector_get(id, Container_SlotToVecIndex(slot));
	}
	
	return 0;
}

//==============================================================================

stock Container_GetParam(id, slot)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
		if(slot < 0 || slot >= Container_GetElementsAmount(id)) // Slot inv�lido
	        return 0;

		return vector_get(id, Container_SlotToVecIndex(slot));
	}

	return 0;
}

//==============================================================================

stock Container_SearchItem(id, itemid)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
	    new aux = Container_GetElementsAmount(id);
	    
	    for(new i = 0; i < aux; i++)
	    {
	    	if(vector_get(id, Container_SlotToVecIndex(i)) == itemid)
	    	    return i;
		}
	}

	return -1;
}

//==============================================================================

stock Container_Empty_Weapons(id)
{
    if(id > 0 && vector_size(id) > 3)
    {
  		new query[128];
        new vec_sqlid = vector_get(id, CONTAINER_INDEX_SQLID);
		new vec_size = vector_size(id);

		for(new i = 4; i < vec_size; i += 2) // Para todos los items almacenados en el vector que simula el contenedor
		{
		    if(GetItemType(vector_get(id, i)) == ITEM_WEAPON) // Como "i" cae en los items, obtenemos el valor en posicion "i" y nos fijamos
			{
			    vector_set(id, CONTAINER_INDEX_USED_SPACE, vector_get(id, CONTAINER_INDEX_USED_SPACE) - GetItemSpace(vector_get(id, i)));
			    format(query, sizeof(query), "DELETE FROM containers_slots WHERE container = %d AND Item = %d AND Param = %d LIMIT 1", vec_sqlid, vector_get(id, i), vector_get(id, i+1)); // Guardamos
                mysql_function_query(dbHandle, query, false, "", "");
				vector_remove(id, i); // Remueve item.
			    vector_remove(id, i); // Al remover item, param cae a la pos de item, por lo tanto borramos la misma.
			    vec_size = vec_size - 2;
			    i = i - 2;
			}
		}
		return 1;
	}

	return 0;
}

//==============================================================================

stock Container_Empty(id)
{
    if(id > 0 && vector_size(id) > 3)
    {
  		new query[128];

        new vec_sqlid = vector_get(id, CONTAINER_INDEX_SQLID);
		new vec_min_item_space = vector_get(id, CONTAINER_INDEX_MIN_ITEM_SPACE);
		new vec_total_space = vector_get(id, CONTAINER_INDEX_TOTAL_SPACE);
		new vec_size = vector_size(id);
		
		for(new i = 4; i < vec_size; i += 2) // Para todos los items almacenados en el vector que simula el contenedor
		{
		    if(GetItemType(vector_get(id, i)) == ITEM_CONTAINER) // Como "i" cae en los items, obtenemos el valor en posicion "i" y nos fijamos
			{
			    Container_Fully_Destroy(vector_get(id, i+1), Container_GetSQLID(vector_get(id, i+1))); // Los items CONTAINER, en su parametro guardan la id del vector real de ejecucion de su contenedor
			}
		}

	    format(query, sizeof(query), "DELETE FROM containers_slots WHERE container = %d", vec_sqlid);
	    mysql_function_query(dbHandle, query, false, "", "");
	    
		vector_clear(id);
		
		vector_push_back(id, vec_sqlid);
		vector_push_back(id, vec_min_item_space);
		vector_push_back(id, vec_total_space);
		vector_push_back(id, 0);
		
		return 1;
	}

	return 0;
}

//==============================================================================

stock Container_AddItemAndParam(id, item, param)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
	    if(GetItemSpace(item) < Container_GetMinItemSpace(id))
	        return 0;
		if(Container_GetFreeSpace(id) < GetItemSpace(item)) // No hay espacio para meter el peso del item
		    return 0;

		vector_push_back(id, item);
		vector_push_back(id, param);

		Container_AddUsedSpace(id, GetItemSpace(item));

		Container_UpdateOpenedDialogs(id);

 		new query[128];

		if(GetItemType(item) == ITEM_CONTAINER)
  		{
			format(query, sizeof(query), "INSERT INTO containers_slots (container, Item, Param) VALUES (%d, %d, %d)", Container_GetSQLID(id), item, Container_GetSQLID(param));
		}
		else
		{
		    format(query, sizeof(query), "INSERT INTO containers_slots (container, Item, Param) VALUES (%d, %d, %d)", Container_GetSQLID(id), item, param);
		}
		mysql_function_query(dbHandle, query, false, "", "");
		
		return 1;
	}

	return 0;
}

//==============================================================================

stock Container_TakeItem(id, slot, &item, &param)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
	    if(slot < 0 || slot >= Container_GetElementsAmount(id)) // Slot inv�lido
	        return 0;

		new vec_index = Container_SlotToVecIndex(slot);

		item = vector_get(id, vec_index);
	    param = vector_get(id, vec_index + 1);

		vector_remove(id, vec_index);
		vector_remove(id, vec_index);

		Container_AddUsedSpace(id, -GetItemSpace(item));

		Container_UpdateOpenedDialogs(id);

 		new query[128];

		if(GetItemType(item) == ITEM_CONTAINER)
		{
			format(query, sizeof(query), "DELETE FROM containers_slots WHERE container = %d AND Item = %d AND Param = %d LIMIT 1", Container_GetSQLID(id), item, Container_GetSQLID(param));
		}
		else
		{
			format(query, sizeof(query), "DELETE FROM containers_slots WHERE container = %d AND Item = %d AND Param = %d LIMIT 1", Container_GetSQLID(id), item, param);
  		}
		mysql_function_query(dbHandle, query, false, "", "");
		
		return 1;
	}

	return 0;
}

//==============================================================================

stock Container_UpdateOpenedDialogs(id)
{
	foreach(new playerid : Player)
	{
	    if(Container_Selection[playerid][csId] == id && Dialog_Opened(playerid)) // Si tiene abierto el dialog de ese contenedor
	    {
	        Dialog_Close(playerid);
		    Container_Show(Container_Selection[playerid][csOriginalId], Container_Selection[playerid][csType], id, playerid);
		}
	}
}

//==============================================================================

stock Container_Show(origid, type, id, playerid, interact=1)
{
	if(id > 0 && vector_size(id) > 3) // Est� creado (tiene al menos los 3 elementos que se generan al crearlo)
	{
	    new string1[64];
	    
	    if(Container_GetElementsAmount(id) > 0)
	    {
			format(Container_DlgInfoStr, sizeof(Container_DlgInfoStr), "%s", "");
			
			new vsize = vector_size(id),
			    slot = 0,
			    item,
			    param;

			for(new i=4; i<vsize; i+=2)
			{
			    item = vector_get(id, i);
			    param = vector_get(id, i+1);
			    if(GetItemType(item) == ITEM_CONTAINER)
			    {
			        format(Container_DlgInfoStr, sizeof(Container_DlgInfoStr), "%s%d - Item: %s\tUsado: %d/%d\tOcupa: %d\n", Container_DlgInfoStr, slot, GetItemName(item), Container_GetUsedSpace(param), Container_GetTotalSpace(param), GetItemSpace(item));
			    }
			    else
			    {
			    	format(Container_DlgInfoStr, sizeof(Container_DlgInfoStr), "%s%d - Item: %s\t%s: %d\tOcupa: %d\n", Container_DlgInfoStr, slot, GetItemName(item), GetItemParamName(item), param, GetItemSpace(item));
			    }
	            slot++;
			}

			switch(type)
			{
			    case CONTAINER_TYPE_TRUNK:
			    {
			        format(string1, sizeof(string1), "[Maletero] Usado: %du - Libre: %du", Container_GetUsedSpace(id), Container_GetTotalSpace(id) - Container_GetUsedSpace(id));
			        if(interact)
					{
			        	Dialog_Show(playerid, Dlg_Show_Trunk_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Tomar", "Cerrar");
					}
					else
					{
						Dialog_Show(playerid, Dlg_Print_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Cerrar", "");
					}
				}
				case CONTAINER_TYPE_INV:
				{
				    format(string1, sizeof(string1), "[Inventario] Usado: %du - Libre: %du", Container_GetUsedSpace(id), Container_GetTotalSpace(id) - Container_GetUsedSpace(id));
			        if(interact)
					{
						Dialog_Show(playerid, Dlg_Show_Inv_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Tomar", "Cerrar");
   					}
					else
					{
						Dialog_Show(playerid, Dlg_Print_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Cerrar", "");
					}
				}
				case CONTAINER_TYPE_HOUSE:
				{
				    format(string1, sizeof(string1), "[Armario] Usado: %du - Libre: %du", Container_GetUsedSpace(id), Container_GetTotalSpace(id) - Container_GetUsedSpace(id));
			        if(interact)
					{
						Dialog_Show(playerid, Dlg_Show_House_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Tomar", "Cerrar");
					}
					else
					{
						Dialog_Show(playerid, Dlg_Print_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Cerrar", "");
					}
				}
				case CONTAINER_TYPE_ITEM:
				{
				    format(string1, sizeof(string1), "[Item] Usado: %du - Libre: %du", Container_GetUsedSpace(id), Container_GetTotalSpace(id) - Container_GetUsedSpace(id));
			        if(interact)
					{
						Dialog_Show(playerid, Dlg_Show_Item_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Tomar", "Cerrar");
					}
					else
					{
						Dialog_Show(playerid, Dlg_Print_Container, DIALOG_STYLE_TABLIST, string1, Container_DlgInfoStr, "Cerrar", "");
					}
				}
				default:
					return 0;
			}
		}
		else
		{
			format(string1, sizeof(string1), "No hay items guardados. Espacio libre: %du", Container_GetTotalSpace(id));
			Dialog_Show(playerid, DLG_SHOW_EMPTY_CONTAINER, DIALOG_STYLE_MSGBOX, "[Vacio]", string1, "Cerrar", "");
		}
		if(interact)
		{
			Container_Selection[playerid][csId] = id;
			Container_Selection[playerid][csOriginalId] = origid;
			Container_Selection[playerid][csType] = type;
		}
		return 1;
	}

	return 0;
}
