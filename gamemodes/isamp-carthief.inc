#if defined _isamp_carthief_included
	#endinput
#endif
#define _isamp_carthief_included

//=========================SISTEMA DE ROBO DE AUTOS=============================

//=================================CONSTANTES===================================

#define POS_CAR_DEMOLITION_X 	2199.0042
#define POS_CAR_DEMOLITION_Y	-2033.5476
#define POS_CAR_DEMOLITION_Z 	13.5339

#define STOLEN_CAR_MISSING_TIME 60 // En minutos, tiempo hasta que vuelva a aparecer una vez desarmado.

#define CMD_BARRETA_COOLDOWN    50 // En minutos
#define CMD_PUENTE_COOLDOWN     50 // En minutos
#define CMD_DESARMAR_COOLDOWN   60 // En minutos
	
//============================FUNCIONES PUBLICAS================================

//============================FUNCIONES INTERNAS================================

//=========================IMPLEMENTACI�N DE FUNCIONES==========================

//================================COMANDOS======================================

CMD:barreta(playerid, params[])
{
	new
		vehicleid = GetClosestVehicle(playerid, 4.0),
   	    Float:pos[3],
	    location[MAX_ZONE_NAME];

	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 7)
		return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes la experiencia necesaria para utilizar este comando.");
	if(ThiefJobInfo[playerid][pForceDoorLimit] != 0)
	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a forzar una cerradura.", ThiefJobInfo[playerid][pForceDoorLimit] / 60);
	    return 1;
	}
	if(IsPlayerInAnyVehicle(playerid))
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�No puedes forzar una cerradura estando arriba del vehiculo!");
	if(GetHandItem(playerid) != ITEM_ID_BARRETA)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes una barreta en tus manos.");
	if(vehicleid == INVALID_VEHICLE_ID)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No estas cerca de ningun vehiculo.");
	if(VehicleInfo[vehicleid][VehLocked] == 0)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Las puertas est� abiertas.");

  	TogglePlayerControllable(playerid, false);
  	PlayerActionMessage(playerid, 15.0, "toma una herramienta y comienza a realizar unas maniobras sobre la manija de la puerta del veh�culo");
    GameTextForPlayer(playerid, "Forzando cerradura, aguarda...", 20 * 1000, 4);
    SetTimerEx("UsarBarreta", 20 * 1000, 0, "ii", playerid, vehicleid);
    
    if(random(10) > 4)
    {
		SendClientMessage(playerid, COLOR_WHITE, "Un civil te vi� forzando la cerradura y alert� a la polic�a. �Apurate o te atrapar�n!");
		GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
		GetPlayer2DZone(playerid, location, MAX_ZONE_NAME);
		foreach(new i : Player)
		{
		    if(isPlayerCopOnDuty(i))
		    {
		        SendClientMessage(i, COLOR_PMA, "[Dpto. de polic�a]: Un civil ha reportado un robo de auto en curso. Hemos marcado la ubicacion en el radar.");
                SendFMessage(i, COLOR_PMA, "[Dpto. de polic�a]: El modelo del veh�culo es un %s y la zona es %s.", GetVehicleName(vehicleid), location);
                SetPlayerCheckpoint(i, pos[0], pos[1], pos[2], 3.0);
			}
		}
	}
	return 1;
}

forward UsarBarreta(playerid, vehicleid);
public UsarBarreta(playerid, vehicleid)
{
    TogglePlayerControllable(playerid, true);

	if(GetHandItem(playerid) != ITEM_ID_BARRETA)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes una barreta en tus manos.");

	if(random(10) > 4)
	{
		if(GetHandParam(playerid) == 1)
		{
			SetHandItemAndParam(playerid, 0, 0);
			DeleteHandItem(playerid);
		} else
			SetHandItemAndParam(playerid, ITEM_ID_BARRETA, GetHandParam(playerid) - 1);
	    SendClientMessage(playerid, COLOR_WHITE, "La barreta cedi� y se rompi�.");
	}
	else
	{
 		VehicleInfo[vehicleid][VehLocked] = 0;
 		SendClientMessage(playerid, COLOR_WHITE, "Forzaste con exito la cerradura.");
		if(ThiefJobInfo[playerid][pFelonLevel] == 7)
 			GiveJobExp(playerid, JOB_FELON, 2);
        ThiefJobInfo[playerid][pForceDoorLimit] = CMD_BARRETA_COOLDOWN * 60;
 	}
	return 1;
}

CMD:puente(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);

	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 7)
		return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes la experiencia necesaria para poder robar autos.");
	if(ThiefJobInfo[playerid][pForceEngineLimit] != 0)
	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a puentear el arranque.", ThiefJobInfo[playerid][pForceEngineLimit] / 60);
	    return 1;
	}
	if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�Debes estar en el asiento de conductor de un veh�culo!");
	GetVehicleParamsEx(vehicleid, VehicleInfo[vehicleid][VehEngine], VehicleInfo[vehicleid][VehLights], VehicleInfo[vehicleid][VehAlarm], vlocked, VehicleInfo[vehicleid][VehBonnet], VehicleInfo[vehicleid][VehBoot], VehicleInfo[vehicleid][VehObjective]);
	if(VehicleInfo[vehicleid][VehEngine] == 1)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "El motor del veh�culo ya se encuentra prendido.");
	    
  	TogglePlayerControllable(playerid, false);
  	PlayerActionMessage(playerid, 15.0, "se inclina hacia abajo en el asiento del conductor y realiza unas maniobras.");
  	GameTextForPlayer(playerid, "Desarmando tambor de arranque, aguarda...", 20 * 1000, 4);
    SetTimerEx("PuenteMotor", 20 * 1000, 0, "ii", playerid, vehicleid);
	return 1;
}

forward PuenteMotor(playerid, vehicleid);
public PuenteMotor(playerid, vehicleid)
{
    TogglePlayerControllable(playerid, true);
    if(random(10) > 4)
    {
        SetEngine(vehicleid, 1);
        SendClientMessage(playerid, COLOR_WHITE, "�Perfecto! Conectas los cables correctos y el motor enciende. �Huye de aqu�!");
		if(ThiefJobInfo[playerid][pFelonLevel] == 7)
 			GiveJobExp(playerid, JOB_FELON, 2);
        ThiefJobInfo[playerid][pForceEngineLimit] = CMD_PUENTE_COOLDOWN * 60;
	}
	else
		SendClientMessage(playerid, COLOR_WHITE, "Mala suerte, has fallado en tu intento por conectar los cables adecuados.");
	return 1;
}

CMD:desarmar(playerid, params[])
{
	new
		vehicleid = GetPlayerVehicleID(playerid),
		Float:vehiclehp,
		repuestos;

	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 7)
		return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes la experiencia necesaria para utilizar este comando.");
	if(ThiefJobInfo[playerid][pDesarmCarLimit] != 0)
	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a desarmar un veh�culo.", ThiefJobInfo[playerid][pDesarmCarLimit] / 60);
	    return 1;
	}
	if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�Debes estar en el asiento del conductor de un veh�culo!");
	if(!IsPlayerInRangeOfPoint(playerid, 5.0, POS_CAR_DEMOLITION_X, POS_CAR_DEMOLITION_Y, POS_CAR_DEMOLITION_Z))
        return SendClientMessage(playerid, COLOR_YELLOW2, "Debes dirigirte al desarmadero con el veh�culo robado.");
	if(VehicleInfo[vehicleid][VehType] != VEH_OWNED)
	    return SendClientMessage(playerid, COLOR_WHITE, "Comprador: Olv�dalo, ese veh�culo no me interesa. (OOC: Solo puedes desarmar veh�culos personales)");
	if(playerHasCarKey(playerid, vehicleid))
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes desarmar un auto del cual tienes las llaves.");
	if(GetHandItem(playerid) != 0)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Tienes tus manos ocupadas y no puedes agarrar los repuestos.");

 	GetVehicleHealth(vehicleid, vehiclehp);
    repuestos = floatround(vehiclehp) / GetItemPrice(ITEM_ID_REPUESTOAUTO) * 2;
    SetHandItemAndParam(playerid, ITEM_ID_REPUESTOAUTO, repuestos);
    LoadHandItem(playerid);
    SetVehicleToRespawn(vehicleid); // Respawn para sacar a los jugadores del veh�culo (no funciona loopear y sacar a los que esten en el veh�culo)
    SetVehiclePos(vehicleid, 8888.0, 8888.0, 888.0);
    SetTimerEx("RespawnDemolishedVehicle", STOLEN_CAR_MISSING_TIME * 60 * 1000, false, "i", vehicleid);
    PlayerActionMessage(playerid, 15.0, "le entrega el veh�culo al empleado del compactador de basura para que lo desarme y demuela.");
    SendFMessage(playerid, COLOR_WHITE, "Comprador: Bien, aqu� tienes %d piezas de repuesto que pude sacarle al auto.", repuestos);
    ThiefJobInfo[playerid][pDesarmCarLimit] = CMD_DESARMAR_COOLDOWN * 60;
	return 1;
}

forward RespawnDemolishedVehicle(vehicleid);
public RespawnDemolishedVehicle(vehicleid)
{
	SetVehicleToRespawn(vehicleid);
	return 1;
}