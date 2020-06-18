.global M7002_expsp_v

M7002_expsp_v:


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
			SMOVIL			-140, R2
			SMOVIL			-1, R3
			SADDA.M2		R3:R2,AR15,AR12
			SNOP 			1
			SSTW			R4,*+AR12[0]		
			SSTW			R5,*+AR12[1]
			SSTW			R6,*+AR12[2]
			SSTW			R7 ,*+AR12[3]
			SSTW			R30 ,*+AR12[4]
			SSTW			R31 ,*+AR12[5]
			SSTW			R32 ,*+AR12[6]
			SSTW			R33 ,*+AR12[7]
			SSTW			R34 ,*+AR12[8]
			SSTW			R35 ,*+AR12[9]
			SSTW			R36 ,*+AR12[10]
			SSTW			R37 ,*+AR12[11]
			SSTW			R38 ,*+AR12[12]
			SSTW			R39 ,*+AR12[13]
			SSTW			R40 ,*+AR12[14]
			SSTW			R41 ,*+AR12[15]
			SSTW			R62 ,*+AR12[16]
			SSTW			R63 ,*+AR12[17]
			SMVAAGL.M1       AR7 , 	R0   
	|		SMVAAGH.M2		  AR7 , R1
			SMVAAGL.M1		  AR8 , R2
	|		SMVAAGH.M2       AR8 , 	R3
			SMVAAGL.M1       AR9 , 	R4   
	|		SMVAAGH.M2		  AR9 , R5
			SMVAAGL.M1		  AR14 , R6
	|		SMVAAGH.M2       AR14 ,	 R7
			SMVAAGL.M1       AR15 ,	 R8  
	|		SMVAAGH.M2		  AR15 , R9
				
			SSTW			R0, *+AR12[18]
			SSTW			R1, *+AR12[19]
			SSTW			R2, *+AR12[20]
			SSTW			R3, *+AR12[21]
			SSTW			R4, *+AR12[22]
			SSTW			R5, *+AR12[23]
			SSTW			R6, *+AR12[24]	   
			SSTW			R7, *+AR12[25]
			SSTW			R8, *+AR12[26]
			SSTW			R9, *+AR12[27]
			  
			SMVAAGL.M1		  OR8 , R0
	|		SMVAAGH.M2       OR8 , 	R1
			SMVAAGL.M1		  OR9 , R2
	|		SMVAAGH.M2       OR9 , 	R3
			SMVAAGL.M1		  OR10 , R4
	|		SMVAAGH.M2       OR10 , R5
			SMVAAGL.M1		  OR15 , R6
	|		SMVAAGH.M2       OR15 ,	 R7
			SSTW			R0, *+AR12[28]
			SSTW			R1, *+AR12[29]
			SSTW			R2, *+AR12[30]
			SSTW			R3, *+AR12[31]
			SSTW			R4, *+AR12[32]
			SSTW			R5, *+AR12[33]	   
			SSTW			R6, *+AR12[34]
			SSTW			R7, *+AR12[35]     
	   


			SMOVIL			3,				R0									;
			SMVCGC			R0,				SCR	
			SNOP			1
			

			SMVAGA36.M1		R13:R12, AR1						; Aaddress
			SMVAGA36.M1		R15:R14, AR2						; Caddress
			SMOV.M1			R10, R57							; Num,
			SNOP			1
			
		
;******************************EXP_CALCULATE************************************************		
		
		
M7002_expsp_v2:
		SMOVIL			16, R8
		SMOVIH 			0x00000000, R8
		SMOVIL 		 	0, R9
		SMVAGA36.M1 	R9:R8,  OR0
		SNOP			1
		VLDDW			*AR1++[OR0], VR47:VR46				; VR47,VR46,V45-input VR43,VR42,VR41-output
		VLDW			*AR1++[OR0], VR45
		VMOVIL			0xAA3B, VR8							; VR8 = l2e
		VMOVIH			0x3FB80000, VR8	
		VMOVIL			0, VR9						
		VMOVIH			0x3f800000, VR9
		SNOP			3
		
		
		VFMULS32.M1		VR47, VR8, VR47						; p = p * l2e
|		VFMULS32.M2		VR46, VR8, VR46
|		VFMULS32.M3		VR45, VR8, VR45
		VMOVIL			0x0000, VR8
		VMOVIH 			0x00000000, VR8
		SNOP 			1
		
		VMOV.M1			VR47, VR33
|		VMOV.M2			VR46, VR32
|		VMOV.M3			VR45, VR31



		VFCMPLS32.M1	VR33,VR8,VR0						; p < 0?
|		VFCMPLS32.M2	VR32,VR8,VR1
|		VFCMPLS32.M3	VR31,VR8,VR2

[VR0]	VFSUBS32.M1		VR9,VR33,VR33						; if (p < 0) then w = p - 1
[VR1]	VFSUBS32.M1		VR9,VR32,VR32
[VR2]	VFSUBS32.M1		VR9,VR31,VR31
		SNOP			2
		
		VFSTRU32.M1		VR33,VR33							; w = int(w)
|		VFSTRU32.M2		VR32,VR32
|		VFSTRU32.M3		VR31,VR31
		SNOP			2
		VFINTS32.M1		VR33,VR33
|		VFINTS32.M2		VR32,VR32
|		VFINTS32.M3		VR31,VR31
		SNOP			2
		
		VFSUBS32.M1		VR33,VR47,VR33						; z = p - w
		VFSUBS32.M1		VR32,VR46,VR32
		VFSUBS32.M1		VR31,VR45,VR31

		VMOVIL			0x82E9, VR8							; p2 = 0.241710325242730
		VMOVIH			0x3E770000, VR8							; p2 = 0.241710325242730
		VMOVIL			0x381D, VR9							; p1 = -0.307068740644571
		VMOVIH			0xBE9D0000, VR9							; p1 = -0.307068740644571
		VMOVIL			0x0001, VR10						; VR10 = 127.000007286727458
		VMOVIH			0x42FE0000, VR10						; VR10 = 127.000007286727458


		VMOV.M1			VR47, VR39
|		VMOV.M2			VR46, VR38
|		VMOV.M3			VR45, VR37
		
		
		VFMULAS32.M1	VR33,VR9,VR39,VR39					; p1 = z * -0.307068740644571 + p
|		VFMULAS32.M2	VR32,VR9,VR38,VR38
|		VFMULAS32.M3	VR31,VR9,VR37,VR37
				
		VFMULS32.M1		VR33,VR33,VR36						; z^2
|		VFMULS32.M2		VR32,VR32,VR35
|		VFMULS32.M3		VR31,VR31,VR34
		SNOP			4
		
		VFMULAS32.M1	VR36,VR8,VR39,VR39					; p2 = z^2 * 0.241710325242730 + p1
|		VFMULAS32.M2	VR35,VR8,VR38,VR38
|		VFMULAS32.M3	VR34,VR8,VR37,VR37
		
		VFMULS32.M1		VR36,VR33,VR36						; z^3
|		VFMULS32.M2		VR35,VR32,VR35
|		VFMULS32.M3		VR34,VR31,VR34
		
		VMOVIL			0x137A, VR8							; p4 = 0.013676518889531
		VMOVIH			0x3C600000, VR8							; p4 = 0.013676518889531
		VMOVIL			0xA09E, VR9							; p3 = 0.051666849136575
		VMOVIH			0x3D530000, VR9							; p3 = 0.051666849136575

		VFMULAS32.M1	VR36,VR9,VR39,VR39					; p3 = z^3 * 0.051666849136575 + p2
|		VFMULAS32.M2	VR35,VR9,VR38,VR38
|		VFMULAS32.M3	VR34,VR9,VR37,VR37

		VFMULS32.M1		VR36,VR33,VR36						; z^4
|		VFMULS32.M2		VR35,VR32,VR35
|		VFMULS32.M3		VR34,VR31,VR34
		SNOP 			4
		
		VFMULAS32.M1	VR36,VR8,VR39,VR39					; p4 = z^4 * 0.013676518889531 + p3
|		VFMULAS32.M2	VR35,VR8,VR38,VR38
|		VFMULAS32.M3	VR34,VR8,VR37,VR37
		SNOP			5
		
		VFADDS32.M1		VR39,VR10,VR39						; p4 = p4 + VR10
		VFADDS32.M1		VR38,VR10,VR38
		VFADDS32.M1		VR37,VR10,VR37
		VMOVIL			0, VR10
		VMOVIH			0x4B000000, VR10
		
		
		VFMULS32.M1		VR39, VR10, VR39					; p4 = p4 * (1<<23)
|		VFMULS32.M2		VR38, VR10, VR38
|		VFMULS32.M3		VR37, VR10, VR37
		SNOP			3
		
		
		VFSTRU32.M1		VR39,VR43
|		VFSTRU32.M2		VR38,VR42
|		VFSTRU32.M3		VR37,VR41
		SNOP			2

		SMOVIL			16, R8
		SMOVIH 			0x00000000, R8
		SMOVIL 		 	0, R9
		SMVAGA36.M1 	R9:R8,  OR0
		SNOP			1
		VSTDW			VR43:VR42, *AR2++[OR0]
		VSTW			VR41, *AR2++[OR0]
		SNOP			3

		SSUBU.M1		48, R57, R57					;
		SLT				0, R57, R1
[R1]	SBR				M7002_expsp_v2	
		SNOP			6
	;; condjump to .M7002_expsp_v2 occurs

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
		SLDW			*+AR12[18], R0
			SLDW			*+AR12[19], R1
			SLDW			*+AR12[20], R2
			SLDW			*+AR12[21], R3
			SLDW			*+AR12[22], R4
			SLDW			*+AR12[23], R5
			SLDW			*+AR12[24], R6
			SLDW			*+AR12[25], R7
			SLDW			*+AR12[26], R8
			SLDW			*+AR12[27], R9
			SNOP			4
			SMVAGA36.M1       R1:R0,AR7    
|			SMVAGA36.M2       R3:R2,AR8  
			SMVAGA36.M1       R5:R4,AR9  
|			SMVAGA36.M2       R7:R6,AR14 
			SMVAGA36.M1       R9:R8,AR15
			
			SLDW			*+AR12[28], R0
			SLDW			*+AR12[29], R1
			SLDW			*+AR12[30], R2
			SLDW			*+AR12[31], R3
			SLDW			*+AR12[32], R4
			SLDW			*+AR12[33], R5
			SLDW			*+AR12[34], R6
			SLDW			*+AR12[35], R7
			SNOP			5
|			SMVAGA36.M2       R1:R0,OR8  
			SMVAGA36.M1       R3:R2,OR9
|			SMVAGA36.M2       R5:R4,OR10
			SMVAGA36.M1       R7:R6,OR15
			
			SLDW			*+AR12[0],R4		
			SLDW			*+AR12[1],R5
			SLDW			*+AR12[2],R6
			SLDW			*+AR12[3],R7 
			SLDW			*+AR12[4],R30 
			SLDW			*+AR12[5],R31
			SLDW			*+AR12[6],R32 
			SLDW			*+AR12[7],R33 
			SLDW			*+AR12[8],R34 
			SLDW			*+AR12[9],R35 
			SLDW			*+AR12[10],R36
			SLDW			*+AR12[11],R37
			SLDW			*+AR12[12],R38
			SLDW			*+AR12[13],R39
			SLDW			*+AR12[14],R40
			SLDW			*+AR12[15],R41
			SLDW			*+AR12[16],R62
			SLDW			*+AR12[17],R63
		
		SBR 			R62
		SNOP 			6
		SWAIT

.size M7002_expsp_v, .-M7002_expsp_v
		
