struct Target_Structure
{
	__int32 Identifier;

	void* Self;

	__int8 Priority;

	float Distance;

	__int32 Tick_Number;
};

std::vector<Target_Structure> Sorted_Target_List;

void* Get_Hitbox_Set(Target_Structure* Target, float(*Bones)[3][4], float Time)
{
	using Setup_Bones_Type = __int8(__thiscall*)(void* Entity, void* Bones, __int32 Maximum_Bones, __int32 Mask, float Current_Time);

	if (Setup_Bones_Type((unsigned __int32)Client_Module + 246656)((void*)((unsigned __int32)Target->Self + 4), Bones, 128, 524032, Time) == 1)
	{
		using Get_Studio_Header_Type = void*(__thiscall*)(void* Entity);

		void* Studio_Header = *(void**)Get_Studio_Header_Type((unsigned __int32)Client_Module + 8512)(Target->Self);

		return (void*)((unsigned __int32)Studio_Header + *(__int32*)((unsigned __int32)Studio_Header + 176));
	}

	return nullptr;
}

void* Original_Copy_Command_Caller;

void __thiscall Redirected_Copy_Command(void* Unknown_Parameter, Command_Structure* Command)
{
	Extended_Command_Structure* Extended_Command = &Extended_Commands[Command->Command_Number % 150];

	Extended_Command->Extra_Commands = 0;

	Global_Variables_Structure* Global_Variables = *(Global_Variables_Structure**)((unsigned __int32)Client_Module + 7096744);

	void* Local_Player = *(void**)((unsigned __int32)Client_Module + 7498712);

	if (Extra_Commands == -1)
	{
		if ((Command->Buttons & 524288) == 524288)
		{
			Extended_Command->Extra_Commands = max(0, Extra_Commands = std::clamp(Interface_Extra_Commands.Integer, (__int32)(0.06f / Global_Variables->Interval_Per_Tick + 0.5f), 21));

			*(__int32*)((unsigned __int32)Local_Player + 16) = min(*(__int32*)((unsigned __int32)Local_Player + 16) + 1, Extended_Command->Extra_Commands * Interface_Interpolate_Extra_Commands.Integer);
		}
		else
		{
			*(__int32*)((unsigned __int32)Local_Player + 16) = max(0, *(__int32*)((unsigned __int32)Local_Player + 16) - 1);
		}
	}

	*(__int32*)((unsigned __int32)Local_Player + 20) = 0;

	if (*(__int8*)((unsigned __int32)Local_Player + 327) == 0)
	{
		using Run_Prediction_Type = void(__cdecl*)();

		Run_Prediction_Type((unsigned __int32)Engine_Module + 527776)();

		float Move_Angles[3] =
		{
			Command->Angles[0],

			Command->Angles[1],

			0
		};

		static float Previous_Move_Angle_Y;

		if ((Command->Buttons & 2) == 2)
		{
			Command->Move[0] = 0;

			if (*(__int32*)((unsigned __int32)Local_Player + 316) == -1)
			{
				Command->Buttons &= ~(*(__int8*)((unsigned __int32)Local_Player + 324) & 2);
			}
			else
			{
				Command->Buttons &= ~(*(__int32*)((unsigned __int32)Local_Player + 5028) & 2);
			}

			float Difference = __builtin_remainderf(Move_Angles[1] - Previous_Move_Angle_Y, 360);

			Previous_Move_Angle_Y = Move_Angles[1];

			float* Velocity = (float*)((unsigned __int32)Local_Player + 256);

			if (__builtin_fabsf(Difference) < __builtin_atan2f(30, __builtin_hypotf(Velocity[0], Velocity[1])) * 180 / 3.1415927f)
			{
				float Strafe_Angle = __builtin_remainderf(Move_Angles[1] - __builtin_atan2f(Velocity[1], Velocity[0]) * 180 / 3.1415927f, 360);

				if (__builtin_signbitf(Strafe_Angle) == 0)
				{
					Command->Move[1] = -400;
				}
				else
				{
					Command->Move[1] = 400;
				}

				Move_Angles[1] -= Strafe_Angle;
			}
			else
			{
				if (__builtin_signbitf(Difference) == 0)
				{
					Command->Move[1] = -400;
				}
				else
				{
					Command->Move[1] = 400;
				}
			}
		}
		else
		{
			Previous_Move_Angle_Y = Move_Angles[1];
		}

		float Previous_Move[2];

		Byte_Manager::Copy_Bytes(0, Previous_Move, sizeof(Previous_Move), Command->Move);

		float Desired_Move_Forward[3];

		float Desired_Move_Right[3];

		Angle_Vectors(Move_Angles, Desired_Move_Forward, Desired_Move_Right, nullptr);

		Desired_Move_Forward[2] = 0;

		Vector_Normalize(Desired_Move_Forward);

		Desired_Move_Right[2] = 0;

		Vector_Normalize(Desired_Move_Right);

		float Desired_Move[2] =
		{
			Desired_Move_Forward[0] * Command->Move[0] + Desired_Move_Right[0] * Command->Move[1],

			Desired_Move_Forward[1] * Command->Move[0] + Desired_Move_Right[1] * Command->Move[1]
		};

		auto Correct_Movement = [&]() -> void
		{
			float Move_Forward[3];

			float Move_Right[3];

			Angle_Vectors(Command->Angles, Move_Forward, Move_Right, nullptr);

			Move_Forward[2] = 0;

			Vector_Normalize(Move_Forward);

			Move_Right[2] = 0;

			Vector_Normalize(Move_Right);

			float Divider = Move_Forward[0] * Move_Right[1] - Move_Right[0] * Move_Forward[1];

			Command->Move[0] = (Desired_Move[0] * Move_Right[1] - Move_Right[0] * Desired_Move[1]) / Divider;

			Command->Move[1] = (Move_Forward[0] * Desired_Move[1] - Desired_Move[0] * Move_Forward[1]) / Divider;
		};

		Correct_Movement();

		void* Network_Channel = *(void**)(*(unsigned __int32*)((unsigned __int32)Engine_Module + 4352236) + 24);

		auto Sequence_Shift = [&](__int32 Reserve)
		{
			if (Extra_Commands < 1)
			{
				*(__int32*)((unsigned __int32)Network_Channel + 8) += (Command->Tick_Number + ~-150) / 150 * 150 + (Reserve * 150);
			}

			*(__int32*)((unsigned __int32)Local_Player + 20) = 1 + (Reserve > 0);
		};

		if (*(__int32*)((unsigned __int32)Local_Player + 228) == 3)
		{
			if (*(__int8*)((unsigned __int32)Local_Player + 7322) == 1)
			{
				if (*(__int32*)((unsigned __int32)Local_Player + 7324) == 0)
				{
					Sequence_Shift(-2);
				}
			}
		}
		else
		{
			if ((*(float*)((unsigned __int32)Local_Player + 4604) + 800 * Global_Variables->Interval_Per_Tick >= 560) + (*(__int8*)((unsigned __int32)Local_Player + 8068) + *(__int8*)((unsigned __int32)Local_Player + 9708)) != 0)
			{
				Sequence_Shift(2);

				Run_Prediction_Type((unsigned __int32)Engine_Module + 527776)();
			}
			else
			{
				void* Prediction = (void*)((unsigned __int32)Client_Module + 8072728);

				*(__int8*)((unsigned __int32)Prediction + 8) = 1;

				*(__int8*)((unsigned __int32)Prediction + 24) = 0;

				using Set_Host_Type = void(__thiscall*)(void* Move_Helper, void* Player);

				Set_Host_Type((unsigned __int32)Client_Module + 1331184)((void*)((unsigned __int32)Client_Module + 7174888), Local_Player);

				Redirected_Run_Command(Prediction, Local_Player, Command, (void*)((unsigned __int32)Client_Module + 7174888));

				Set_Host_Type((unsigned __int32)Client_Module + 1331184)((void*)((unsigned __int32)Client_Module + 7174888), nullptr);

				*(__int8*)((unsigned __int32)Prediction + 8) = 0;

				__int32 Block_Buttons = 2049;

				if ((*(__int32*)((unsigned __int32)Local_Player + 324) & 9) == 0)
				{
					if ((*(__int32*)((unsigned __int32)Local_Player + 5020) & 32) == 0)
					{
						using Can_Attack_Type = __int8(__thiscall*)(void* Player);

						if (Can_Attack_Type((unsigned __int32)Client_Module + 2541696)(Local_Player) == 1)
						{
							void* Weapon = *(__int16*)((unsigned __int32)Local_Player + 7866) == 0 ? *(void**)((unsigned __int32)Client_Module + 7644532 + (((*(unsigned __int32*)((unsigned __int32)Local_Player + 4228) & 4095) - 4097) << 4)) : nullptr;

							if (Weapon == nullptr)
							{
								Block_Buttons = 0;
							}
							else
							{
								__int32 Entity_Number = 1;

								using Get_Interpolation_Time_Type = float(__cdecl*)();

								float Interpolation_Time = Get_Interpolation_Time_Type((unsigned __int32)Engine_Module + 594000)();

								Sorted_Target_List.clear();

								auto Get_Target_Time = [&](Target_Structure* Target) -> float
								{
									if (Interface_Target_On_Simulation.Integer == 0)
									{
										return *(float*)((unsigned __int32)Local_Player + 336);
									}

									return *(float*)((unsigned __int32)Target->Self + 336);
								};

								Traverse_Entity_List_Label:
								{
									void* Entity = *(void**)((unsigned __int32)Client_Module + 7644532 + ((Entity_Number - 4097) << 4));

									if (Entity != nullptr)
									{
										if (Entity != Local_Player)
										{
											__int32 Identifier = Get_Identifier(Entity, 0, 0);

											if (Identifier != -1)
											{
												if (*(__int32*)((unsigned __int32)Entity + 228) % 2 != *(__int32*)((unsigned __int32)Local_Player + 228) % 2)
												{
													float* Local_Player_Origin = (float*)((unsigned __int32)Local_Player + 292);

													float* Entity_Origin = (float*)((unsigned __int32)Entity + 292);

													float Time = *(float*)((unsigned __int32)Entity + 336) + Interpolation_Time;

													Target_Structure Target =
													{
														Identifier,

														Entity,

														(__int8)(Identifier == 270 ? *(void**)((unsigned __int32)Entity + 8040) == *(void**)((unsigned __int32)Local_Player + 376) : 0),

														__builtin_powf(Local_Player_Origin[0] - Entity_Origin[0], 2) + __builtin_powf(Local_Player_Origin[1] - Entity_Origin[1], 2) + __builtin_powf(Local_Player_Origin[2] - Entity_Origin[2], 2),

														(__int32)(Time / Global_Variables->Interval_Per_Tick + 0.5f)
													};

													if (*(float*)((unsigned __int32)Entity + 16) != Get_Target_Time(&Target))
													{
														if (Identifier == 277)
														{
															if (*(float*)((unsigned __int32)Entity + 4844) == 1)
															{
																Sorted_Target_List.push_back(Target);
															}
														}
														else
														{
															Sorted_Target_List.push_back(Target);
														}
													}
												}
											}
										}
									}

									if (Entity_Number != *(__int32*)((unsigned __int32)Client_Module + 7644568))
									{
										Entity_Number += 1;

										goto Traverse_Entity_List_Label;
									}
								}

								auto Target_List_Sort_Prepare = [](Target_Structure& X, Target_Structure& Y) -> __int8
								{
									return X.Priority < Y.Priority;
								};

								std::sort(Sorted_Target_List.begin(), Sorted_Target_List.end(), Target_List_Sort_Prepare);

								auto Target_List_Sort_Finish = [](Target_Structure& X, Target_Structure& Y) -> __int8
								{
									if (X.Priority > Y.Priority)
									{
										return 1;
									}

									return X.Distance < Y.Distance;
								};

								std::sort(Sorted_Target_List.begin(), Sorted_Target_List.end(), Target_List_Sort_Finish);

								size_t Target_Number = 0;

								using Get_Weapon_Data_Type = void*(__thiscall*)(void* Weapon);

								void* Weapon_Data = Get_Weapon_Data_Type((unsigned __int32)Client_Module + 86432)(Weapon);

								__int8 Is_Melee = *(__int32*)((unsigned __int32)Weapon_Data + 352) * (*(__int32*)((unsigned __int32)Weapon_Data + 348) ^ 1) <= 1;

								__int8 Healing = *(void**)((unsigned __int32)Local_Player + 8076) != INVALID_HANDLE_VALUE;

								__int32 Weapon_Identifier = Get_Identifier(Weapon, 1, 0);

								__int8 Is_Cold_Melee = Weapon_Identifier == 231;

								using Get_Eye_Position_Type = void(__thiscall*)(void* Entity, float* Eye_Position);

								float Eye_Position[3];

								Get_Eye_Position_Type((unsigned __int32)Client_Module + 108512)(Local_Player, Eye_Position);

								__int8 Cancelable_Shove = 1 + (*(float*)((unsigned __int32)Local_Player + 7336) < Global_Variables->Current_Time);

								Target_Structure* Shove_Target = nullptr;

								Shove_Traverse_Sorted_Target_List_Label:
								{
									if (Target_Number != Sorted_Target_List.size())
									{
										Target_Structure* Target = &Sorted_Target_List.at(Target_Number);

										__int8 Forced = 0;

										if (Is_Melee + Healing != 0)
										{
											if ((Target->Identifier ^ 72) % 348 >= 72)
											{
												Forced = 1;

												goto Shove_Label;
											}
										}

										if ((Target->Identifier >= 263) + (Target->Identifier <= 270 - Is_Cold_Melee - (*(__int8*)((unsigned __int32)Local_Player + 8070) ^ 1)) == 2)
										{
											Shove_Label:
											{
												__int8 Infected = (Target->Identifier == 264) * (Forced ^ 1);

												if ((Infected ^ 1) + (*(__int32*)((unsigned __int32)Target->Self + 52) == 15) != 0)
												{
													using Get_Sequence_Name_Type = char*(__thiscall*)(void* Entity, __int32 Sequence);

													if (__builtin_strstr(Get_Sequence_Name_Type((unsigned __int32)Client_Module + 203392)(Target->Self, *(__int32*)((unsigned __int32)Target->Self + 2212)), "hove") == nullptr)
													{
														using Perform_Shove_Trace = __int8(__thiscall*)(void* Weapon, float* Direction);

														using Get_Center_Type = float*(__thiscall*)(void* Entity);

														float* Target_Origin = Get_Center_Type((unsigned __int32)Client_Module + 114400)(Target->Self);

														float Direction[3] =
														{
															Target_Origin[0] - Eye_Position[0],

															Target_Origin[1] - Eye_Position[1],

															Target_Origin[2] - Eye_Position[2]
														};

														Vector_Normalize(Direction);

														*(float*)((unsigned __int32)Weapon + 2724) = 75;

														*(__int32*)((unsigned __int32)Weapon + 3248) = 0;

														Perform_Trace_Target = (void*)((unsigned __int32)Target->Self ^ 1);

														Perform_Shove_Trace((unsigned __int32)Client_Module + 3220512)(Weapon, Direction);

														if (Perform_Trace_Target == Target->Self)
														{
															Command->Tick_Number = Target->Tick_Number;

															Command->Angles[0] = __builtin_atan2f(-Direction[2], __builtin_hypotf(Direction[0], Direction[1])) * 180 / 3.1415927f;

															Command->Angles[1] = __builtin_atan2f(Direction[1], Direction[0]) * 180 / 3.1415927f;

															if (Cancelable_Shove == 1)
															{
																float Shove_Multiplier = min((Global_Variables->Current_Time - *(float*)((unsigned __int32)Weapon + 2704) + *(float*)((unsigned __int32)Weapon + 2700)) / *(float*)((unsigned __int32)Weapon + 2700), 1.f);

																Command->Angles[1] += -45 * Shove_Multiplier + 45 * (1 - Shove_Multiplier);
															}
															else
															{
																Command->Angles[1] += 45;
															}

															Command->Buttons |= 2048;

															Block_Buttons = 1;

															if (Interface_Riot_Deprioritize.Integer == 1)
															{
																Cancelable_Shove = Infected * 2;
															}

															*(float*)((unsigned __int32)Target->Self + 16) = Get_Target_Time(Target);

															goto Shove_Found_Target_Label;
														}
													}
												}
											}
										}

										Target_Number += 1;

										goto Shove_Traverse_Sorted_Target_List_Label;

										Shove_Found_Target_Label:
										{
											Shove_Target = Target;
										}
									}
								}

								if (Cancelable_Shove != 0)
								{
									__int8 Can_Attack = (*(float*)((unsigned __int32)Weapon + 2400) <= Global_Variables->Current_Time) * (*(__int32*)((unsigned __int32)Weapon + 2436) > 0 - Is_Melee * 2) * (*(float*)((unsigned __int32)Local_Player + 3872) <= Global_Variables->Current_Time);

									if (Can_Attack == 1)
									{
										if (Weapon_Identifier != 96)
										{
											Target_Structure* Aim_Target = nullptr;

											if (Is_Melee + Healing == 0)
											{
												Target_Number = 0;

												Aim_Traverse_Sorted_Target_List_Label:
												{
													if (Target_Number != Sorted_Target_List.size())
													{
														Target_Structure* Target = &Sorted_Target_List.at(Target_Number);

														float Bones[128][3][4];

														void* Hitbox_Set = Get_Hitbox_Set(Target, Bones, Global_Variables->Current_Time);

														if (Hitbox_Set != nullptr)
														{
															auto Perform_Trace = [&](float* Angles) -> __int8
															{
																using Fire_Bullet_Type = void(__thiscall*)(void* Player, float X, float Y, float Z, float* Angles, __int32 Identifier, void* Unknown_Parameter);

																__int32 Bullets = *(__int32*)((unsigned __int32)Weapon_Data + 2520);

																*(__int32*)((unsigned __int32)Weapon_Data + 2520) = 1;

																Perform_Trace_Target = (void*)((unsigned __int32)Target->Self ^ 1);

																void* Previous_Audio_Device = *(void**)((unsigned __int32)Engine_Module + 5050008);

																*(void**)((unsigned __int32)Engine_Module + 5050008) = nullptr;

																*(__int32*)((unsigned __int32)Client_Module + 7683464) = 1;

																*(__int32*)((unsigned __int32)Client_Module + 8175320) = 0;

																*(__int32*)((unsigned __int32)Client_Module + 8175392) = 0;

																using Get_Weapon_Identifier_Type = __int32(__thiscall**)(void* Weapon);

																Fire_Bullet_Type((unsigned __int32)Client_Module + 3103776)(Local_Player, Eye_Position[0], Eye_Position[1], Eye_Position[2], Angles, (*Get_Weapon_Identifier_Type(*(unsigned __int32*)Weapon + 1532))(Weapon), nullptr);

																*(__int32*)((unsigned __int32)Client_Module + 8175392) = 1;

																*(__int32*)((unsigned __int32)Client_Module + 8175320) = 1;

																*(__int32*)((unsigned __int32)Client_Module + 7683464) = 0;

																*(void**)((unsigned __int32)Engine_Module + 5050008) = Previous_Audio_Device;

																*(__int32*)((unsigned __int32)Weapon_Data + 2520) = Bullets;

																return Perform_Trace_Target == Target->Self;
															};

															static std::unordered_map<__int32, __int32> Hitboxes =
															{
																{ 0, 10 },

																{ 13, 0 },

																{ 99, 9 },

																{ 263, 10 },

																{ 264, 15 },

																{ 265, 4 },

																{ 270, 10 },

																{ 272, 4 },

																{ 276, 12 },

																{ 277, 10 }
															};

															void* Hitbox = (void*)((unsigned __int32)Hitbox_Set + 12 + Hitboxes[Target->Identifier] * 68);

															float* Hitbox_Minimum = (float*)((unsigned __int32)Hitbox + 8);

															float* Hitbox_Maximum = (float*)((unsigned __int32)Hitbox + 20);

															float Hitbox_Center[3]
															{
																(Hitbox_Minimum[0] + Hitbox_Maximum[0]) / 2,

																(Hitbox_Minimum[1] + Hitbox_Maximum[1]) / 2,

																(Hitbox_Minimum[2] + Hitbox_Maximum[2]) / 2
															};

															float Target_Origin[3] =
															{
																Bones[*(__int32*)Hitbox][0][0] * Hitbox_Center[0] + Bones[*(__int32*)Hitbox][0][1] * Hitbox_Center[1] + Bones[*(__int32*)Hitbox][0][2] * Hitbox_Center[2] + Bones[*(__int32*)Hitbox][0][3],

																Bones[*(__int32*)Hitbox][1][0] * Hitbox_Center[0] + Bones[*(__int32*)Hitbox][1][1] * Hitbox_Center[1] + Bones[*(__int32*)Hitbox][1][2] * Hitbox_Center[2] + Bones[*(__int32*)Hitbox][1][3],

																Bones[*(__int32*)Hitbox][2][0] * Hitbox_Center[0] + Bones[*(__int32*)Hitbox][2][1] * Hitbox_Center[1] + Bones[*(__int32*)Hitbox][2][2] * Hitbox_Center[2] + Bones[*(__int32*)Hitbox][2][3]
															};

															float Direction[3] =
															{
																Target_Origin[0] - Eye_Position[0],

																Target_Origin[1] - Eye_Position[1],

																Target_Origin[2] - Eye_Position[2]
															};

															Vector_Normalize(Direction);

															float Angles[3] =
															{
																__builtin_atan2f(-Direction[2], __builtin_hypotf(Direction[0], Direction[1])) * 180 / 3.1415927f,

																__builtin_atan2f(Direction[1], Direction[0]) * 180 / 3.1415927f,

																0
															};

															if (Perform_Trace(Angles) == 1)
															{
																Command->Tick_Number = Target->Tick_Number;

																Byte_Manager::Copy_Bytes(0, Command->Angles, sizeof(Angles), Angles);

																Command->Buttons |= 1;

																*(float*)((unsigned __int32)Target->Self + 16) = Get_Target_Time(Target);

																goto Aim_Found_Target_Label;
															}
														}

														Target_Number += 1;

														goto Aim_Traverse_Sorted_Target_List_Label;

														Aim_Found_Target_Label:
														{
															Aim_Target = Target;
														}
													}
												}
											}

											__int8 Compensate_Burst = 0;

											if (Weapon_Identifier == 153)
											{
												if (*(float*)((unsigned __int32)Weapon + 3392) + *(float*)((unsigned __int32)Weapon + 3396) != 0)
												{
													Compensate_Burst = 1;

													Command->Buttons |= 1;
												}
											}

											if ((Command->Buttons & 1) == 1)
											{
												if (Shove_Target != nullptr)
												{
													*(float*)((unsigned __int32)Shove_Target->Self + 16) = 0;

													if (Aim_Target != nullptr)
													{
														*(float*)((unsigned __int32)Aim_Target->Self + 16) = Get_Target_Time(Aim_Target);
													}
												}

												if (Is_Melee == 0)
												{
													*(__int32*)((unsigned __int32)Client_Module + 7075944) = Command->Random_Seed;

													using Random_Type = float(__cdecl*)(char* Name, float Minimum, float Maximum, void* Unknown_Parameter);

													using Update_Spread_Type = void(__thiscall*)(void* Weapon);

													if (Compensate_Burst == 0)
													{
														Update_Spread_Type((unsigned __int32)Client_Module + 3197648)(Weapon);
													}

													float Maximum_Spread = *(float*)((unsigned __int32)Weapon + 3340);

													float* Recoil = (float*)((unsigned __int32)Local_Player + 4612);

													Command->Angles[0] -= Random_Type((unsigned __int32)Client_Module + 1756592)((char*)"CTerrorGun::FireBullet HorizSpread", -Maximum_Spread, Maximum_Spread, nullptr) + Recoil[0];

													Command->Angles[1] -= Random_Type((unsigned __int32)Client_Module + 1756592)((char*)"CTerrorGun::FireBullet VertSpread", -Maximum_Spread, Maximum_Spread, nullptr) + Recoil[1];

													Command->Angles[2] -= Recoil[2];
												}

												Block_Buttons = 2048;
											}
										}
									}

									if (Block_Buttons == 2049)
									{
										Block_Buttons = 2048 * (Cancelable_Shove == 1) + (Can_Attack ^ 1);
									}
								}
							}
						}
					}
				}

				Command->Buttons &= ~Block_Buttons;

				Byte_Manager::Copy_Bytes(0, Command->Move, sizeof(Previous_Move), Previous_Move);

				Correct_Movement();
			}

			*(__int8*)((unsigned __int32)__builtin_frame_address(0) + 235) = Extra_Commands <= 0;
		}
	}	

	Command->Buttons &= ~524288;

	(decltype(&Redirected_Copy_Command)(Original_Copy_Command_Caller))(Unknown_Parameter, Command);
}