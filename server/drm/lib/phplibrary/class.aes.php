<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.aes.php
// Date: 22.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

/*
KeySize = 32 Byte = 256 Bit
BlockSize = 16 Byte = 128 Bit
*/

class CAES
{
	var $aRcon, $aForwardTable, $aLastForwardTable, $aInverseTable, $aLastInverseTable;

	function CAES()
	{
		$this->aRcon = array(
			0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020,
			0x00000040, 0x00000080, 0x0000001B, 0x00000036, 0x0000006C, 0x000000D8,
			0x000000AB, 0x0000004D, 0x0000009A, 0x0000002F, 0x0000005E, 0x000000BC,
			0x00000063, 0x000000C6, 0x00000097, 0x00000035, 0x0000006A, 0x000000D4,
			0x000000B3, 0x0000007D, 0x000000FA, 0x000000EF, 0x000000C5, 0x00000091
		);

		$this->aForwardTable = array(
			0xA56363C6, 0x847C7CF8, 0x997777EE, 0x8D7B7BF6, 0x0DF2F2FF, 0xBD6B6BD6, 0xB16F6FDE, 0x54C5C591,
			0x50303060, 0x03010102, 0xA96767CE, 0x7D2B2B56, 0x19FEFEE7, 0x62D7D7B5, 0xE6ABAB4D, 0x9A7676EC,
			0x45CACA8F, 0x9D82821F, 0x40C9C989, 0x877D7DFA, 0x15FAFAEF, 0xEB5959B2, 0xC947478E, 0x0BF0F0FB,
			0xECADAD41, 0x67D4D4B3, 0xFDA2A25F, 0xEAAFAF45, 0xBF9C9C23, 0xF7A4A453, 0x967272E4, 0x5BC0C09B,
			0xC2B7B775, 0x1CFDFDE1, 0xAE93933D, 0x6A26264C, 0x5A36366C, 0x413F3F7E, 0x02F7F7F5, 0x4FCCCC83,
			0x5C343468, 0xF4A5A551, 0x34E5E5D1, 0x08F1F1F9, 0x937171E2, 0x73D8D8AB, 0x53313162, 0x3F15152A,
			0x0C040408, 0x52C7C795, 0x65232346, 0x5EC3C39D, 0x28181830, 0xA1969637, 0x0F05050A, 0xB59A9A2F,
			0x0907070E, 0x36121224, 0x9B80801B, 0x3DE2E2DF, 0x26EBEBCD, 0x6927274E, 0xCDB2B27F, 0x9F7575EA,
			0x1B090912, 0x9E83831D, 0x742C2C58, 0x2E1A1A34, 0x2D1B1B36, 0xB26E6EDC, 0xEE5A5AB4, 0xFBA0A05B,
			0xF65252A4, 0x4D3B3B76, 0x61D6D6B7, 0xCEB3B37D, 0x7B292952, 0x3EE3E3DD, 0x712F2F5E, 0x97848413,
			0xF55353A6, 0x68D1D1B9, 0x00000000, 0x2CEDEDC1, 0x60202040, 0x1FFCFCE3, 0xC8B1B179, 0xED5B5BB6,
			0xBE6A6AD4, 0x46CBCB8D, 0xD9BEBE67, 0x4B393972, 0xDE4A4A94, 0xD44C4C98, 0xE85858B0, 0x4ACFCF85,
			0x6BD0D0BB, 0x2AEFEFC5, 0xE5AAAA4F, 0x16FBFBED, 0xC5434386, 0xD74D4D9A, 0x55333366, 0x94858511,
			0xCF45458A, 0x10F9F9E9, 0x06020204, 0x817F7FFE, 0xF05050A0, 0x443C3C78, 0xBA9F9F25, 0xE3A8A84B,
			0xF35151A2, 0xFEA3A35D, 0xC0404080, 0x8A8F8F05, 0xAD92923F, 0xBC9D9D21, 0x48383870, 0x04F5F5F1,
			0xDFBCBC63, 0xC1B6B677, 0x75DADAAF, 0x63212142, 0x30101020, 0x1AFFFFE5, 0x0EF3F3FD, 0x6DD2D2BF,
			0x4CCDCD81, 0x140C0C18, 0x35131326, 0x2FECECC3, 0xE15F5FBE, 0xA2979735, 0xCC444488, 0x3917172E,
			0x57C4C493, 0xF2A7A755, 0x827E7EFC, 0x473D3D7A, 0xAC6464C8, 0xE75D5DBA, 0x2B191932, 0x957373E6,
			0xA06060C0, 0x98818119, 0xD14F4F9E, 0x7FDCDCA3, 0x66222244, 0x7E2A2A54, 0xAB90903B, 0x8388880B,
			0xCA46468C, 0x29EEEEC7, 0xD3B8B86B, 0x3C141428, 0x79DEDEA7, 0xE25E5EBC, 0x1D0B0B16, 0x76DBDBAD,
			0x3BE0E0DB, 0x56323264, 0x4E3A3A74, 0x1E0A0A14, 0xDB494992, 0x0A06060C, 0x6C242448, 0xE45C5CB8,
			0x5DC2C29F, 0x6ED3D3BD, 0xEFACAC43, 0xA66262C4, 0xA8919139, 0xA4959531, 0x37E4E4D3, 0x8B7979F2,
			0x32E7E7D5, 0x43C8C88B, 0x5937376E, 0xB76D6DDA, 0x8C8D8D01, 0x64D5D5B1, 0xD24E4E9C, 0xE0A9A949,
			0xB46C6CD8, 0xFA5656AC, 0x07F4F4F3, 0x25EAEACF, 0xAF6565CA, 0x8E7A7AF4, 0xE9AEAE47, 0x18080810,
			0xD5BABA6F, 0x887878F0, 0x6F25254A, 0x722E2E5C, 0x241C1C38, 0xF1A6A657, 0xC7B4B473, 0x51C6C697,
			0x23E8E8CB, 0x7CDDDDA1, 0x9C7474E8, 0x211F1F3E, 0xDD4B4B96, 0xDCBDBD61, 0x868B8B0D, 0x858A8A0F,
			0x907070E0, 0x423E3E7C, 0xC4B5B571, 0xAA6666CC, 0xD8484890, 0x05030306, 0x01F6F6F7, 0x120E0E1C,
			0xA36161C2, 0x5F35356A, 0xF95757AE, 0xD0B9B969, 0x91868617, 0x58C1C199, 0x271D1D3A, 0xB99E9E27,
			0x38E1E1D9, 0x13F8F8EB, 0xB398982B, 0x33111122, 0xBB6969D2, 0x70D9D9A9, 0x898E8E07, 0xA7949433,
			0xB69B9B2D, 0x221E1E3C, 0x92878715, 0x20E9E9C9, 0x49CECE87, 0xFF5555AA, 0x78282850, 0x7ADFDFA5,
			0x8F8C8C03, 0xF8A1A159, 0x80898909, 0x170D0D1A, 0xDABFBF65, 0x31E6E6D7, 0xC6424284, 0xB86868D0,
			0xC3414182, 0xB0999929, 0x772D2D5A, 0x110F0F1E, 0xCBB0B07B, 0xFC5454A8, 0xD6BBBB6D, 0x3A16162C
		);

		$this->aLastForwardTable = array(
			0x00000063, 0x0000007C, 0x00000077, 0x0000007B, 0x000000F2, 0x0000006B, 0x0000006F, 0x000000C5,
			0x00000030, 0x00000001, 0x00000067, 0x0000002B, 0x000000FE, 0x000000D7, 0x000000AB, 0x00000076,
			0x000000CA, 0x00000082, 0x000000C9, 0x0000007D, 0x000000FA, 0x00000059, 0x00000047, 0x000000F0,
			0x000000AD, 0x000000D4, 0x000000A2, 0x000000AF, 0x0000009C, 0x000000A4, 0x00000072, 0x000000C0,
			0x000000B7, 0x000000FD, 0x00000093, 0x00000026, 0x00000036, 0x0000003F, 0x000000F7, 0x000000CC,
			0x00000034, 0x000000A5, 0x000000E5, 0x000000F1, 0x00000071, 0x000000D8, 0x00000031, 0x00000015,
			0x00000004, 0x000000C7, 0x00000023, 0x000000C3, 0x00000018, 0x00000096, 0x00000005, 0x0000009A,
			0x00000007, 0x00000012, 0x00000080, 0x000000E2, 0x000000EB, 0x00000027, 0x000000B2, 0x00000075,
			0x00000009, 0x00000083, 0x0000002C, 0x0000001A, 0x0000001B, 0x0000006E, 0x0000005A, 0x000000A0,
			0x00000052, 0x0000003B, 0x000000D6, 0x000000B3, 0x00000029, 0x000000E3, 0x0000002F, 0x00000084,
			0x00000053, 0x000000D1, 0x00000000, 0x000000ED, 0x00000020, 0x000000FC, 0x000000B1, 0x0000005B,
			0x0000006A, 0x000000CB, 0x000000BE, 0x00000039, 0x0000004A, 0x0000004C, 0x00000058, 0x000000CF,
			0x000000D0, 0x000000EF, 0x000000AA, 0x000000FB, 0x00000043, 0x0000004D, 0x00000033, 0x00000085,
			0x00000045, 0x000000F9, 0x00000002, 0x0000007F, 0x00000050, 0x0000003C, 0x0000009F, 0x000000A8,
			0x00000051, 0x000000A3, 0x00000040, 0x0000008F, 0x00000092, 0x0000009D, 0x00000038, 0x000000F5,
			0x000000BC, 0x000000B6, 0x000000DA, 0x00000021, 0x00000010, 0x000000FF, 0x000000F3, 0x000000D2,
			0x000000CD, 0x0000000C, 0x00000013, 0x000000EC, 0x0000005F, 0x00000097, 0x00000044, 0x00000017,
			0x000000C4, 0x000000A7, 0x0000007E, 0x0000003D, 0x00000064, 0x0000005D, 0x00000019, 0x00000073,
			0x00000060, 0x00000081, 0x0000004F, 0x000000DC, 0x00000022, 0x0000002A, 0x00000090, 0x00000088,
			0x00000046, 0x000000EE, 0x000000B8, 0x00000014, 0x000000DE, 0x0000005E, 0x0000000B, 0x000000DB,
			0x000000E0, 0x00000032, 0x0000003A, 0x0000000A, 0x00000049, 0x00000006, 0x00000024, 0x0000005C,
			0x000000C2, 0x000000D3, 0x000000AC, 0x00000062, 0x00000091, 0x00000095, 0x000000E4, 0x00000079,
			0x000000E7, 0x000000C8, 0x00000037, 0x0000006D, 0x0000008D, 0x000000D5, 0x0000004E, 0x000000A9,
			0x0000006C, 0x00000056, 0x000000F4, 0x000000EA, 0x00000065, 0x0000007A, 0x000000AE, 0x00000008,
			0x000000BA, 0x00000078, 0x00000025, 0x0000002E, 0x0000001C, 0x000000A6, 0x000000B4, 0x000000C6,
			0x000000E8, 0x000000DD, 0x00000074, 0x0000001F, 0x0000004B, 0x000000BD, 0x0000008B, 0x0000008A,
			0x00000070, 0x0000003E, 0x000000B5, 0x00000066, 0x00000048, 0x00000003, 0x000000F6, 0x0000000E,
			0x00000061, 0x00000035, 0x00000057, 0x000000B9, 0x00000086, 0x000000C1, 0x0000001D, 0x0000009E,
			0x000000E1, 0x000000F8, 0x00000098, 0x00000011, 0x00000069, 0x000000D9, 0x0000008E, 0x00000094,
			0x0000009B, 0x0000001E, 0x00000087, 0x000000E9, 0x000000CE, 0x00000055, 0x00000028, 0x000000DF,
			0x0000008C, 0x000000A1, 0x00000089, 0x0000000D, 0x000000BF, 0x000000E6, 0x00000042, 0x00000068,
			0x00000041, 0x00000099, 0x0000002D, 0x0000000F, 0x000000B0, 0x00000054, 0x000000BB, 0x00000016
		);

		$this->aInverseTable = array(
			0x50A7F451, 0x5365417E, 0xC3A4171A, 0x965E273A, 0xCB6BAB3B, 0xF1459D1F, 0xAB58FAAC, 0x9303E34B,
			0x55FA3020, 0xF66D76AD, 0x9176CC88, 0x254C02F5, 0xFCD7E54F, 0xD7CB2AC5, 0x80443526, 0x8FA362B5,
			0x495AB1DE, 0x671BBA25, 0x980EEA45, 0xE1C0FE5D, 0x02752FC3, 0x12F04C81, 0xA397468D, 0xC6F9D36B,
			0xE75F8F03, 0x959C9215, 0xEB7A6DBF, 0xDA595295, 0x2D83BED4, 0xD3217458, 0x2969E049, 0x44C8C98E,
			0x6A89C275, 0x78798EF4, 0x6B3E5899, 0xDD71B927, 0xB64FE1BE, 0x17AD88F0, 0x66AC20C9, 0xB43ACE7D,
			0x184ADF63, 0x82311AE5, 0x60335197, 0x457F5362, 0xE07764B1, 0x84AE6BBB, 0x1CA081FE, 0x942B08F9,
			0x58684870, 0x19FD458F, 0x876CDE94, 0xB7F87B52, 0x23D373AB, 0xE2024B72, 0x578F1FE3, 0x2AAB5566,
			0x0728EBB2, 0x03C2B52F, 0x9A7BC586, 0xA50837D3, 0xF2872830, 0xB2A5BF23, 0xBA6A0302, 0x5C8216ED,
			0x2B1CCF8A, 0x92B479A7, 0xF0F207F3, 0xA1E2694E, 0xCDF4DA65, 0xD5BE0506, 0x1F6234D1, 0x8AFEA6C4,
			0x9D532E34, 0xA055F3A2, 0x32E18A05, 0x75EBF6A4, 0x39EC830B, 0xAAEF6040, 0x069F715E, 0x51106EBD,
			0xF98A213E, 0x3D06DD96, 0xAE053EDD, 0x46BDE64D, 0xB58D5491, 0x055DC471, 0x6FD40604, 0xFF155060,
			0x24FB9819, 0x97E9BDD6, 0xCC434089, 0x779ED967, 0xBD42E8B0, 0x888B8907, 0x385B19E7, 0xDBEEC879,
			0x470A7CA1, 0xE90F427C, 0xC91E84F8, 0x00000000, 0x83868009, 0x48ED2B32, 0xAC70111E, 0x4E725A6C,
			0xFBFF0EFD, 0x5638850F, 0x1ED5AE3D, 0x27392D36, 0x64D90F0A, 0x21A65C68, 0xD1545B9B, 0x3A2E3624,
			0xB1670A0C, 0x0FE75793, 0xD296EEB4, 0x9E919B1B, 0x4FC5C080, 0xA220DC61, 0x694B775A, 0x161A121C,
			0x0ABA93E2, 0xE52AA0C0, 0x43E0223C, 0x1D171B12, 0x0B0D090E, 0xADC78BF2, 0xB9A8B62D, 0xC8A91E14,
			0x8519F157, 0x4C0775AF, 0xBBDD99EE, 0xFD607FA3, 0x9F2601F7, 0xBCF5725C, 0xC53B6644, 0x347EFB5B,
			0x7629438B, 0xDCC623CB, 0x68FCEDB6, 0x63F1E4B8, 0xCADC31D7, 0x10856342, 0x40229713, 0x2011C684,
			0x7D244A85, 0xF83DBBD2, 0x1132F9AE, 0x6DA129C7, 0x4B2F9E1D, 0xF330B2DC, 0xEC52860D, 0xD0E3C177,
			0x6C16B32B, 0x99B970A9, 0xFA489411, 0x2264E947, 0xC48CFCA8, 0x1A3FF0A0, 0xD82C7D56, 0xEF903322,
			0xC74E4987, 0xC1D138D9, 0xFEA2CA8C, 0x360BD498, 0xCF81F5A6, 0x28DE7AA5, 0x268EB7DA, 0xA4BFAD3F,
			0xE49D3A2C, 0x0D927850, 0x9BCC5F6A, 0x62467E54, 0xC2138DF6, 0xE8B8D890, 0x5EF7392E, 0xF5AFC382,
			0xBE805D9F, 0x7C93D069, 0xA92DD56F, 0xB31225CF, 0x3B99ACC8, 0xA77D1810, 0x6E639CE8, 0x7BBB3BDB,
			0x097826CD, 0xF418596E, 0x01B79AEC, 0xA89A4F83, 0x656E95E6, 0x7EE6FFAA, 0x08CFBC21, 0xE6E815EF,
			0xD99BE7BA, 0xCE366F4A, 0xD4099FEA, 0xD67CB029, 0xAFB2A431, 0x31233F2A, 0x3094A5C6, 0xC066A235,
			0x37BC4E74, 0xA6CA82FC, 0xB0D090E0, 0x15D8A733, 0x4A9804F1, 0xF7DAEC41, 0x0E50CD7F, 0x2FF69117,
			0x8DD64D76, 0x4DB0EF43, 0x544DAACC, 0xDF0496E4, 0xE3B5D19E, 0x1B886A4C, 0xB81F2CC1, 0x7F516546,
			0x04EA5E9D, 0x5D358C01, 0x737487FA, 0x2E410BFB, 0x5A1D67B3, 0x52D2DB92, 0x335610E9, 0x1347D66D,
			0x8C61D79A, 0x7A0CA137, 0x8E14F859, 0x893C13EB, 0xEE27A9CE, 0x35C961B7, 0xEDE51CE1, 0x3CB1477A,
			0x59DFD29C, 0x3F73F255, 0x79CE1418, 0xBF37C773, 0xEACDF753, 0x5BAAFD5F, 0x146F3DDF, 0x86DB4478,
			0x81F3AFCA, 0x3EC468B9, 0x2C342438, 0x5F40A3C2, 0x72C31D16, 0x0C25E2BC, 0x8B493C28, 0x41950DFF,
			0x7101A839, 0xDEB30C08, 0x9CE4B4D8, 0x90C15664, 0x6184CB7B, 0x70B632D5, 0x745C6C48, 0x4257B8D0
		);

		$this->aLastInverseTable = array(
			0x00000052, 0x00000009, 0x0000006A, 0x000000D5, 0x00000030, 0x00000036, 0x000000A5, 0x00000038,
			0x000000BF, 0x00000040, 0x000000A3, 0x0000009E, 0x00000081, 0x000000F3, 0x000000D7, 0x000000FB,
			0x0000007C, 0x000000E3, 0x00000039, 0x00000082, 0x0000009B, 0x0000002F, 0x000000FF, 0x00000087,
			0x00000034, 0x0000008E, 0x00000043, 0x00000044, 0x000000C4, 0x000000DE, 0x000000E9, 0x000000CB,
			0x00000054, 0x0000007B, 0x00000094, 0x00000032, 0x000000A6, 0x000000C2, 0x00000023, 0x0000003D,
			0x000000EE, 0x0000004C, 0x00000095, 0x0000000B, 0x00000042, 0x000000FA, 0x000000C3, 0x0000004E,
			0x00000008, 0x0000002E, 0x000000A1, 0x00000066, 0x00000028, 0x000000D9, 0x00000024, 0x000000B2,
			0x00000076, 0x0000005B, 0x000000A2, 0x00000049, 0x0000006D, 0x0000008B, 0x000000D1, 0x00000025,
			0x00000072, 0x000000F8, 0x000000F6, 0x00000064, 0x00000086, 0x00000068, 0x00000098, 0x00000016,
			0x000000D4, 0x000000A4, 0x0000005C, 0x000000CC, 0x0000005D, 0x00000065, 0x000000B6, 0x00000092,
			0x0000006C, 0x00000070, 0x00000048, 0x00000050, 0x000000FD, 0x000000ED, 0x000000B9, 0x000000DA,
			0x0000005E, 0x00000015, 0x00000046, 0x00000057, 0x000000A7, 0x0000008D, 0x0000009D, 0x00000084,
			0x00000090, 0x000000D8, 0x000000AB, 0x00000000, 0x0000008C, 0x000000BC, 0x000000D3, 0x0000000A,
			0x000000F7, 0x000000E4, 0x00000058, 0x00000005, 0x000000B8, 0x000000B3, 0x00000045, 0x00000006,
			0x000000D0, 0x0000002C, 0x0000001E, 0x0000008F, 0x000000CA, 0x0000003F, 0x0000000F, 0x00000002,
			0x000000C1, 0x000000AF, 0x000000BD, 0x00000003, 0x00000001, 0x00000013, 0x0000008A, 0x0000006B,
			0x0000003A, 0x00000091, 0x00000011, 0x00000041, 0x0000004F, 0x00000067, 0x000000DC, 0x000000EA,
			0x00000097, 0x000000F2, 0x000000CF, 0x000000CE, 0x000000F0, 0x000000B4, 0x000000E6, 0x00000073,
			0x00000096, 0x000000AC, 0x00000074, 0x00000022, 0x000000E7, 0x000000AD, 0x00000035, 0x00000085,
			0x000000E2, 0x000000F9, 0x00000037, 0x000000E8, 0x0000001C, 0x00000075, 0x000000DF, 0x0000006E,
			0x00000047, 0x000000F1, 0x0000001A, 0x00000071, 0x0000001D, 0x00000029, 0x000000C5, 0x00000089,
			0x0000006F, 0x000000B7, 0x00000062, 0x0000000E, 0x000000AA, 0x00000018, 0x000000BE, 0x0000001B,
			0x000000FC, 0x00000056, 0x0000003E, 0x0000004B, 0x000000C6, 0x000000D2, 0x00000079, 0x00000020,
			0x0000009A, 0x000000DB, 0x000000C0, 0x000000FE, 0x00000078, 0x000000CD, 0x0000005A, 0x000000F4,
			0x0000001F, 0x000000DD, 0x000000A8, 0x00000033, 0x00000088, 0x00000007, 0x000000C7, 0x00000031,
			0x000000B1, 0x00000012, 0x00000010, 0x00000059, 0x00000027, 0x00000080, 0x000000EC, 0x0000005F,
			0x00000060, 0x00000051, 0x0000007F, 0x000000A9, 0x00000019, 0x000000B5, 0x0000004A, 0x0000000D,
			0x0000002D, 0x000000E5, 0x0000007A, 0x0000009F, 0x00000093, 0x000000C9, 0x0000009C, 0x000000EF,
			0x000000A0, 0x000000E0, 0x0000003B, 0x0000004D, 0x000000AE, 0x0000002A, 0x000000F5, 0x000000B0,
			0x000000C8, 0x000000EB, 0x000000BB, 0x0000003C, 0x00000083, 0x00000053, 0x00000099, 0x00000061,
			0x00000017, 0x0000002B, 0x00000004, 0x0000007E, 0x000000BA, 0x00000077, 0x000000D6, 0x00000026,
			0x000000E1, 0x00000069, 0x00000014, 0x00000063, 0x00000055, 0x00000021, 0x0000000C, 0x0000007D
		);
	}

	function G($t, $x, $y)
	{
		return $t[MathAnd(MathShR($x, $y), 255)];
	}

	function H($w0, $w1, $w2, $w3, $x)
	{
		return MathXor($w0, MathOr(MathShL($w1, 8), MathShR($w1, 24)), MathOr(MathShL($w2, 16), MathShR($w2, 16)), MathOr(MathShL($w3, 24), MathShR($w3, 8)), $x);
	}

	function I($x, $u)
	{
		//0x7F7F7F7F, 0x1B1B1B1B
		return MathXor(MathShL(MathAnd($x, 2139062143), 1), MathAnd(MathSub($u, MathShR($u, 7)), 454761243));
	}

	function J($x)
	{
		//0x80808080
		return MathAnd($x, '2155905152');
	}

	function K($f2, $f4, $f8, $f9)
	{
		return MathAnd(MathXor($f2, $f4, $f8, MathOr(MathShL(MathXor($f2, $f9), 24), MathShR(MathXor($f2, $f9), 8)), MathOr(MathShL(MathXor($f4, $f9), 16), MathShR(MathXor($f4, $f9), 16)), MathOr(MathShL($f9, 8), MathShR($f9, 24))), '4294967295');
	}

	function ExpandKeyForEncryption($sKey)
	{
		$sKey = SubStr($sKey, 0, 32);

		for($i = 0; $i < 32; $i++)
		 $aKey[$i] = 0;
		for($i = 0; $i < StrLen($sKey); $i++)
			$aKey[$i] = Ord($sKey[$i]);

		for($i = 0; $i < 64; $i++)
			$aExKey[$i] = 0;
		for($i = 0; $i < 8; $i++)
			$aExKey[$i] = $aKey[$i * 4] + $aKey[$i * 4 + 1] * 256 + $aKey[$i * 4 + 2] * 65536 + $aKey[$i * 4 + 3] * 16777216;

		$i = 0;
		$j = 0;

		do
		{
			$t = MathOr(MathShL($aExKey[$i + 7], 24), MathShR($aExKey[$i + 7], 8));

			$w0 = $this->aLastForwardTable[MathAnd(MathShR($t,  0), 255)];
			$w1 = $this->aLastForwardTable[MathAnd(MathShR($t,  8), 255)];
			$w2 = $this->aLastForwardTable[MathAnd(MathShR($t, 16), 255)];
			$w3 = $this->aLastForwardTable[MathAnd(MathShR($t, 24), 255)];
			$aExKey[$i +  8] = MathXor($aExKey[$i], $w0, MathOr(MathShL($w1, 8), MathShR($w1, 24)), MathOr(MathShL($w2, 16), MathShR($w2, 16)), MathOr(MathShL($w3, 24), MathShR($w3, 8)), $this->aRcon[$j]);
			$aExKey[$i +  9] = MathXor($aExKey[$i + 1], $aExKey[$i +  8]);
			$aExKey[$i + 10] = MathXor($aExKey[$i + 2], $aExKey[$i +  9]);
			$aExKey[$i + 11] = MathXor($aExKey[$i + 3], $aExKey[$i + 10]);
			$j = $j + 1;

			$t = $aExKey[$i + 11];
			$w0 = $this->aLastForwardTable[MathAnd(MathShR($t,  0), 255)];
			$w1 = $this->aLastForwardTable[MathAnd(MathShR($t,  8), 255)];
			$w2 = $this->aLastForwardTable[MathAnd(MathShR($t, 16), 255)];
			$w3 = $this->aLastForwardTable[MathAnd(MathShR($t, 24), 255)];
			$aExKey[$i + 12] = MathXor($aExKey[$i + 4], $w0, MathOr(($w1 << 8), ($w1 >> 24)), MathOr(($w2 << 16), ($w2 >> 16)), MathOr(($w3 << 24), ($w3 >> 8)));
			$aExKey[$i + 13] = MathXor($aExKey[$i + 5], $aExKey[$i + 12]);
			$aExKey[$i + 14] = MathXor($aExKey[$i + 6], $aExKey[$i + 13]);
			$aExKey[$i + 15] = MathXor($aExKey[$i + 7], $aExKey[$i + 14]);
			$i = $i + 8;
		}
		while($i < 52);

		return $aExKey;
	}

	function ExpandKeyForDecryption($sKey)
	{
		$aExKey = $this->ExpandKeyForEncryption($sKey);

		for($i = 1; $i < 14; $i++)
		{
			$f9 = $aExKey[$i * 4];
			$u = $this->J($f9);
			$f2 = $this->I($f9, $u);
			$u = $this->J($f2);
			$f4 = $this->I($f2, $u);
			$u = $this->J($f4);
			$f8 = $this->I($f4, $u);
			$f9 = MathXor($f9, $f8);
			$aExKey[$i * 4] = $this->K($f2, $f4, $f8, $f9);
			$f9 = $aExKey[$i * 4 + 1];
			$u = $this->J($f9);
			$f2 = $this->I($f9, $u);
			$u = $this->J($f2);
			$f4 = $this->I($f2, $u);
			$u = $this->J($f4);
			$f8 = $this->I($f4, $u);
			$f9 = MathXor($f9, $f8);
			$aExKey[$i * 4 + 1] = $this->K($f2, $f4, $f8, $f9);
			$f9 = $aExKey[$i * 4 + 2];
			$u = $this->J($f9);
			$f2 = $this->I($f9, $u);
			$u = $this->J($f2);
			$f4 = $this->I($f2, $u);
			$u = $this->J($f4);
			$f8 = $this->I($f4, $u);
			$f9 = MathXor($f9, $f8);
			$aExKey[$i * 4 + 2] = $this->K($f2, $f4, $f8, $f9);
			$f9 = $aExKey[$i * 4 + 3];
			$u = $this->J($f9);
			$f2 = $this->I($f9, $u);
			$u = $this->J($f2);
			$f4 = $this->I($f2, $u);
			$u = $this->J($f4);
			$f8 = $this->I($f4, $u);
			$f9 = MathXor($f9, $f8);
			$aExKey[$i * 4 + 3] = $this->K($f2, $f4, $f8, $f9);
		}

		return $aExKey;
	}

	function BlockEncrypt($aExKey, $sBlock)
	{
		$sBlock = SubStr($sBlock, 0, 16);		

		for($i = 0; $i < 16; $i++)
			$aInBuf[$i] = 0;
		for($i = 0; $i < 16; $i++)
			$aOutBuf[$i] = 0;

		for($i = 0; $i < StrLen($sBlock); $i++)
			$aInBuf[$i] = Ord($sBlock[$i]);

		for($i = 0; $i < 4; $i++)
			$aT0[$i] = $aInBuf[$i * 4] + $aInBuf[$i * 4 + 1] * 256 + $aInBuf[$i * 4 + 2] * 65536 + $aInBuf[$i * 4 + 3] * 16777216;

		for($i = 0; $i < 4; $i++)
			$aT0[$i] = MathXor($aT0[$i], $aExKey[$i]);

		// Round 1

		$w0 = $this->G($this->aForwardTable, $aT0[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[3], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[4]);

		$w0 = $this->G($this->aForwardTable, $aT0[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[0], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[5]);

		$w0 = $this->G($this->aForwardTable, $aT0[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[1], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[6]);

		$w0 = $this->G($this->aForwardTable, $aT0[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[2], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[7]);

		// Round 2

		$w0 = $this->G($this->aForwardTable, $aT1[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[3], 24);
			
		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[8]);

		$w0 = $this->G($this->aForwardTable, $aT1[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[0], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[9]);

		$w0 = $this->G($this->aForwardTable, $aT1[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[1], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[10]);

		$w0 = $this->G($this->aForwardTable, $aT1[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[2], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[11]);

		// Round 3

		$w0 = $this->G($this->aForwardTable, $aT0[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[3], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[12]);

		$w0 = $this->G($this->aForwardTable, $aT0[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[0], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[13]);

		$w0 = $this->G($this->aForwardTable, $aT0[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[1], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[14]);

		$w0 = $this->G($this->aForwardTable, $aT0[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[2], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[15]);

		// Round 4

		$w0 = $this->G($this->aForwardTable, $aT1[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[3], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[16]);

		$w0 = $this->G($this->aForwardTable, $aT1[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[0], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[17]);

		$w0 = $this->G($this->aForwardTable, $aT1[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[1], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[18]);

		$w0 = $this->G($this->aForwardTable, $aT1[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[2], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[19]);

		// Round 5

		$w0 = $this->G($this->aForwardTable, $aT0[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[3], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[20]);

		$w0 = $this->G($this->aForwardTable, $aT0[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[0], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[21]);

		$w0 = $this->G($this->aForwardTable, $aT0[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[1], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[22]);

		$w0 = $this->G($this->aForwardTable, $aT0[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[2], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[23]);

		// Round 6

		$w0 = $this->G($this->aForwardTable, $aT1[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[3], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[24]);

		$w0 = $this->G($this->aForwardTable, $aT1[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[0], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[25]);

		$w0 = $this->G($this->aForwardTable, $aT1[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[1], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[26]);

		$w0 = $this->G($this->aForwardTable, $aT1[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[2], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[27]);

		// Round 7

		$w0 = $this->G($this->aForwardTable, $aT0[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[3], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[28]);

		$w0 = $this->G($this->aForwardTable, $aT0[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[0], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[29]);

		$w0 = $this->G($this->aForwardTable, $aT0[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[1], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[30]);

		$w0 = $this->G($this->aForwardTable, $aT0[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[2], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[31]);

		// Round 8

		$w0 = $this->G($this->aForwardTable, $aT1[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[3], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[32]);

		$w0 = $this->G($this->aForwardTable, $aT1[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[0], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[33]);

		$w0 = $this->G($this->aForwardTable, $aT1[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[1], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[34]);

		$w0 = $this->G($this->aForwardTable, $aT1[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[2], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[35]);

		// Round 9

		$w0 = $this->G($this->aForwardTable, $aT0[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[3], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[36]);

		$w0 = $this->G($this->aForwardTable, $aT0[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[0], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[37]);

		$w0 = $this->G($this->aForwardTable, $aT0[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[1], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[38]);

		$w0 = $this->G($this->aForwardTable, $aT0[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[2], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[39]);

		// Round 10

		$w0 = $this->G($this->aForwardTable, $aT1[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[3], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[40]);

		$w0 = $this->G($this->aForwardTable, $aT1[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[0], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[41]);

		$w0 = $this->G($this->aForwardTable, $aT1[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[1], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[42]);

		$w0 = $this->G($this->aForwardTable, $aT1[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[2], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[43]);

		// Round 11

		$w0 = $this->G($this->aForwardTable, $aT0[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[3], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[44]);

		$w0 = $this->G($this->aForwardTable, $aT0[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[0], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[45]);

		$w0 = $this->G($this->aForwardTable, $aT0[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[1], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[46]);

		$w0 = $this->G($this->aForwardTable, $aT0[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[2], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[47]);

		// Round 12

		$w0 = $this->G($this->aForwardTable, $aT1[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[3], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[48]);

		$w0 = $this->G($this->aForwardTable, $aT1[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[0], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[49]);

		$w0 = $this->G($this->aForwardTable, $aT1[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[1], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[50]);

		$w0 = $this->G($this->aForwardTable, $aT1[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT1[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT1[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT1[2], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[51]);

		// Round 13

		$w0 = $this->G($this->aForwardTable, $aT0[0], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[1], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[2], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[3], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[52]);

		$w0 = $this->G($this->aForwardTable, $aT0[1], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[2], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[3], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[0], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[53]);

		$w0 = $this->G($this->aForwardTable, $aT0[2], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[3], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[0], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[1], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[54]);

		$w0 = $this->G($this->aForwardTable, $aT0[3], 0);
		$w1 = $this->G($this->aForwardTable, $aT0[0], 8);
		$w2 = $this->G($this->aForwardTable, $aT0[1], 16);
		$w3 = $this->G($this->aForwardTable, $aT0[2], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[55]);

		// Round 14

		$w0 = $this->G($this->aLastForwardTable, $aT1[0], 0);
		$w1 = $this->G($this->aLastForwardTable, $aT1[1], 8);
		$w2 = $this->G($this->aLastForwardTable, $aT1[2], 16);
		$w3 = $this->G($this->aLastForwardTable, $aT1[3], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[56]);

		$w0 = $this->G($this->aLastForwardTable, $aT1[1], 0);
		$w1 = $this->G($this->aLastForwardTable, $aT1[2], 8);
		$w2 = $this->G($this->aLastForwardTable, $aT1[3], 16);
		$w3 = $this->G($this->aLastForwardTable, $aT1[0], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[57]);

		$w0 = $this->G($this->aLastForwardTable, $aT1[2], 0);
		$w1 = $this->G($this->aLastForwardTable, $aT1[3], 8);
		$w2 = $this->G($this->aLastForwardTable, $aT1[0], 16);
		$w3 = $this->G($this->aLastForwardTable, $aT1[1], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[58]);

		$w0 = $this->G($this->aLastForwardTable, $aT1[3], 0);
		$w1 = $this->G($this->aLastForwardTable, $aT1[0], 8);
		$w2 = $this->G($this->aLastForwardTable, $aT1[1], 16);
		$w3 = $this->G($this->aLastForwardTable, $aT1[2], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[59]);

		for($i = 0; $i < 4; $i++)
		{
			$aOutBuf[$i * 4 + 0] = MathAnd(MathShR($aT0[$i], 0), 255);
			$aOutBuf[$i * 4 + 1] = MathAnd(MathShR($aT0[$i], 8), 255);
			$aOutBuf[$i * 4 + 2] = MathAnd(MathShR($aT0[$i], 16), 255);
			$aOutBuf[$i * 4 + 3] = MathAnd(MathShR($aT0[$i], 24), 255);
		}

		$sResult = '';
		for($i = 0; $i < 16; $i++)
		  $sResult .= Chr($aOutBuf[$i]);

		return $sResult;
	}

	function BlockDecrypt($aExKey, $sBlock)
	{
		$sBlock = SubStr($sBlock, 0, 16);		

		for($i = 0; $i < 16; $i++)
			$aInBuf[$i] = 0;
		for($i = 0; $i < 16; $i++)
			$aOutBuf[$i] = 0;

		for($i = 0; $i < StrLen($sBlock); $i++)
			$aInBuf[$i] = Ord($sBlock[$i]);

		for($i = 0; $i < 4; $i++)
			$aT0[$i] = $aInBuf[$i * 4] + $aInBuf[$i * 4 + 1] * 256 + $aInBuf[$i * 4 + 2] * 65536 + $aInBuf[$i * 4 + 3] * 16777216;

		for($i = 0; $i < 4; $i++)
			$aT0[$i] = MathXor($aT0[$i], $aExKey[$i + 56]);

		// Round 1

		$w0 = $this->G($this->aInverseTable, $aT0[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[1], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[52]);

		$w0 = $this->G($this->aInverseTable, $aT0[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[2], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[53]);

		$w0 = $this->G($this->aInverseTable, $aT0[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[3], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[54]);

		$w0 = $this->G($this->aInverseTable, $aT0[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[0], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[55]);

		// Round 2

		$w0 = $this->G($this->aInverseTable, $aT1[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[1], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[48]);

		$w0 = $this->G($this->aInverseTable, $aT1[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[2], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[49]);

		$w0 = $this->G($this->aInverseTable, $aT1[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[3], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[50]);

		$w0 = $this->G($this->aInverseTable, $aT1[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[0], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[51]);

		// Round 3

		$w0 = $this->G($this->aInverseTable, $aT0[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[1], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[44]);

		$w0 = $this->G($this->aInverseTable, $aT0[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[2], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[45]);

		$w0 = $this->G($this->aInverseTable, $aT0[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[3], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[46]);

		$w0 = $this->G($this->aInverseTable, $aT0[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[0], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[47]);

		// Round 4

		$w0 = $this->G($this->aInverseTable, $aT1[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[1], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[40]);

		$w0 = $this->G($this->aInverseTable, $aT1[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[2], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[41]);

		$w0 = $this->G($this->aInverseTable, $aT1[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[3], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[42]);

		$w0 = $this->G($this->aInverseTable, $aT1[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[0], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[43]);

		// Round 5

		$w0 = $this->G($this->aInverseTable, $aT0[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[1], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[36]);

		$w0 = $this->G($this->aInverseTable, $aT0[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[2], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[37]);

		$w0 = $this->G($this->aInverseTable, $aT0[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[3], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[38]);

		$w0 = $this->G($this->aInverseTable, $aT0[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[0], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[39]);

		// Round 6

		$w0 = $this->G($this->aInverseTable, $aT1[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[1], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[32]);

		$w0 = $this->G($this->aInverseTable, $aT1[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[2], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[33]);

		$w0 = $this->G($this->aInverseTable, $aT1[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[3], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[34]);

		$w0 = $this->G($this->aInverseTable, $aT1[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[0], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[35]);

		// Round 7

		$w0 = $this->G($this->aInverseTable, $aT0[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[1], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[28]);

		$w0 = $this->G($this->aInverseTable, $aT0[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[2], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[29]);

		$w0 = $this->G($this->aInverseTable, $aT0[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[3], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[30]);

		$w0 = $this->G($this->aInverseTable, $aT0[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[0], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[31]);

		// Round 8

		$w0 = $this->G($this->aInverseTable, $aT1[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[1], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[24]);

		$w0 = $this->G($this->aInverseTable, $aT1[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[2], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[25]);

		$w0 = $this->G($this->aInverseTable, $aT1[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[3], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[26]);

		$w0 = $this->G($this->aInverseTable, $aT1[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[0], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[27]);

		// Round 9

		$w0 = $this->G($this->aInverseTable, $aT0[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[1], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[20]);

		$w0 = $this->G($this->aInverseTable, $aT0[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[2], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[21]);

		$w0 = $this->G($this->aInverseTable, $aT0[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[3], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[22]);

		$w0 = $this->G($this->aInverseTable, $aT0[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[0], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[23]);

		// Round 10

		$w0 = $this->G($this->aInverseTable, $aT1[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[1], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[16]);

		$w0 = $this->G($this->aInverseTable, $aT1[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[2], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[17]);

		$w0 = $this->G($this->aInverseTable, $aT1[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[3], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[18]);

		$w0 = $this->G($this->aInverseTable, $aT1[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[0], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[19]);

		// Round 11

		$w0 = $this->G($this->aInverseTable, $aT0[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[1], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[12]);

		$w0 = $this->G($this->aInverseTable, $aT0[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[2], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[13]);

		$w0 = $this->G($this->aInverseTable, $aT0[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[3], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[14]);

		$w0 = $this->G($this->aInverseTable, $aT0[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[0], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[15]);

		// Round 12

		$w0 = $this->G($this->aInverseTable, $aT1[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[1], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[8]);

		$w0 = $this->G($this->aInverseTable, $aT1[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[2], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[9]);

		$w0 = $this->G($this->aInverseTable, $aT1[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[3], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[10]);

		$w0 = $this->G($this->aInverseTable, $aT1[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT1[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT1[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT1[0], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[11]);

		// Round 13

		$w0 = $this->G($this->aInverseTable, $aT0[0], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[3], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[2], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[1], 24);

		$aT1[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[4]);

		$w0 = $this->G($this->aInverseTable, $aT0[1], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[0], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[3], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[2], 24);

		$aT1[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[5]);

		$w0 = $this->G($this->aInverseTable, $aT0[2], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[1], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[0], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[3], 24);

		$aT1[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[6]);

		$w0 = $this->G($this->aInverseTable, $aT0[3], 0);
		$w1 = $this->G($this->aInverseTable, $aT0[2], 8);
		$w2 = $this->G($this->aInverseTable, $aT0[1], 16);
		$w3 = $this->G($this->aInverseTable, $aT0[0], 24);

		$aT1[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[7]);

		// Round 14

		$w0 = $this->G($this->aLastInverseTable, $aT1[0], 0);
		$w1 = $this->G($this->aLastInverseTable, $aT1[3], 8);
		$w2 = $this->G($this->aLastInverseTable, $aT1[2], 16);
		$w3 = $this->G($this->aLastInverseTable, $aT1[1], 24);

		$aT0[0] = $this->H($w0, $w1, $w2, $w3, $aExKey[0]);

		$w0 = $this->G($this->aLastInverseTable, $aT1[1], 0);
		$w1 = $this->G($this->aLastInverseTable, $aT1[0], 8);
		$w2 = $this->G($this->aLastInverseTable, $aT1[3], 16);
		$w3 = $this->G($this->aLastInverseTable, $aT1[2], 24);

		$aT0[1] = $this->H($w0, $w1, $w2, $w3, $aExKey[1]);

		$w0 = $this->G($this->aLastInverseTable, $aT1[2], 0);
		$w1 = $this->G($this->aLastInverseTable, $aT1[1], 8);
		$w2 = $this->G($this->aLastInverseTable, $aT1[0], 16);
		$w3 = $this->G($this->aLastInverseTable, $aT1[3], 24);

		$aT0[2] = $this->H($w0, $w1, $w2, $w3, $aExKey[2]);

		$w0 = $this->G($this->aLastInverseTable, $aT1[3], 0);
		$w1 = $this->G($this->aLastInverseTable, $aT1[2], 8);
		$w2 = $this->G($this->aLastInverseTable, $aT1[1], 16);
		$w3 = $this->G($this->aLastInverseTable, $aT1[0], 24);

		$aT0[3] = $this->H($w0, $w1, $w2, $w3, $aExKey[3]);

		for($i = 0; $i < 4; $i++)
		{
			$aOutBuf[$i * 4 + 0] = MathAnd(MathShR($aT0[$i], 0), 255);
			$aOutBuf[$i * 4 + 1] = MathAnd(MathShR($aT0[$i], 8), 255);
			$aOutBuf[$i * 4 + 2] = MathAnd(MathShR($aT0[$i], 16), 255);
			$aOutBuf[$i * 4 + 3] = MathAnd(MathShR($aT0[$i], 24), 255);
		}

		$sResult = '';
		for($i = 0; $i < 16; $i++)
		  $sResult .= Chr($aOutBuf[$i]);

		return $sResult;
	}

	//Public

	function Encrypt($sKey, $sMessage)
	{
		$sResult = '';

		//Size
		for($i = 0; $i < 4; $i++)
			$sResult .= Chr(StrLen($sMessage) >> ($i * 8) % 256);

		//Data
		$aExKey = $this->ExpandKeyForEncryption($sKey);
		for($i = 0; $i < StrLen($sMessage); $i += 16)
			$sResult .= $this->BlockEncrypt($aExKey, SubStr($sMessage, $i, 16));
		
		return $sResult;
	}

	function Decrypt($sKey, $sMessage)
	{
		$sResult = '';

		//Size
		$iSize = 0;
		for($i = 0; $i < 4; $i++)
			$iSize += Ord($sMessage[$i]) << ($i * 8);

		//Data
		$aExKey = $this->ExpandKeyForDecryption($sKey);
		for($i = 4; $i < $iSize; $i += 16)
			$sResult .= $this->BlockDecrypt($aExKey, SubStr($sMessage, $i, 16));

		$sResult = SubStr($sResult, 0, $iSize);

		return $sResult;
	}

}

function TestClassAES()
{
	$aTests[] = '$cAES = new CAES; return MathStrToHex($cAES->Encrypt("MyKey", "MyMessage")) == "90000008a08011a480e3d5304689fed8a224e90";';
	$aTests[] = '$cAES = new CAES; return $cAES->Decrypt("MyKey", MathHexToStr("90000008a08011a480e3d5304689fed8a224e90")) == "MyMessage";';

	return Test('ClassAES', $aTests);
}

?>