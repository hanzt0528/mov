#include "xbox.h"


void* memcpy_b2l(void*pDst, void*pSrc, size_t size)
{

	uint8_t *pD = (uint8_t *)pDst;
	uint8_t *pS = (uint8_t *)pSrc;

	for (size_t i = 0; i < size; i++)
	{
		pD[i] = pS[size - 1 - i];
	}

	return NULL;
}


void* memcpy_l2b(void*pDst, void*pSrc, size_t size)
{

	uint8_t *pD = (uint8_t *)pDst;
	uint8_t *pS = (uint8_t *)pSrc;

	for (int i = size - 1; i >= 0; i--)
	{
		pD[i] = pS[size - 1 - i];
	}

	return NULL;
}


int prototype_parser(char *pszfilename)
{
	FILE *pf = nullptr;

	x_mov_file movfile;

	//fopen_s(&pf, "T:\\footage\\UHD\\20170424165112\\High\\节目01_20170424165112_1.mov", "rb");
	errno_t err = fopen_s(&pf, pszfilename, "rb");

	if (err != 0)
	{
		return err;
	}

	_fseeki64(pf, 0, SEEK_END);

	uint64_t ui64FileSize = _ftelli64(pf);
	char *pReadCurrent;
	uint64_t ui64CurrentReadPos = 0;

	uint64_t ui64lastreadbufferlength = 1024;

	pReadCurrent = new char[ui64lastreadbufferlength];

	while (ui64CurrentReadPos < ui64FileSize)
	{
		_fseeki64(pf, ui64CurrentReadPos, SEEK_SET);

		fread(pReadCurrent, 1, 8, pf);

		xbox box;

		memcpy_b2l(&box.size, pReadCurrent, 4);

		memcpy_b2l(&box.type, pReadCurrent + 4, 4);


		if (box.type == 'ftyp')
		{
			_fseeki64(pf, ui64CurrentReadPos, SEEK_SET);

			if (box.size > ui64lastreadbufferlength)
			{
				delete[]pReadCurrent;

				pReadCurrent = new char[box.size];

				ui64lastreadbufferlength = box.size;
			}

			fread(pReadCurrent, 1, box.size, pf);

			xbox_file_type ft;

			memcpy_b2l(&ft.size, pReadCurrent, 4);

			memcpy_b2l(&ft.type, pReadCurrent + 4, 4);

			memcpy_b2l(&ft.major_brand, pReadCurrent + 8, 4);

			memcpy_b2l(&ft.minor_version, pReadCurrent + 12, 4);

			ft.compatible_brands_count = (ft.size - 16) / 4;
			ft.compatible_brands = new uint32_t*[ft.compatible_brands_count];

			for (auto i = 0; i < ft.compatible_brands_count; i++)
			{
				ft.compatible_brands[i] = new uint32_t;

				memcpy_b2l(ft.compatible_brands[i], pReadCurrent + 16 + i * 4, 4);
			}

			if (*ft.compatible_brands[0] == 'qt  ')
			{
				//it's quick time file standard.
			}

			ui64CurrentReadPos += box.size;
		}

		if (box.type == 'free')
		{
			ui64CurrentReadPos += box.size;
		}

		if (box.type == 'skip')
		{
			ui64CurrentReadPos += box.size;
		}

		if (box.type == 'moov')
		{
			xbox moov;

			_fseeki64(pf, ui64CurrentReadPos, SEEK_SET);

			if (box.size > ui64lastreadbufferlength)
			{
				delete[]pReadCurrent;

				pReadCurrent = new char[box.size];

				ui64lastreadbufferlength = box.size;
			}

			char * pMoovStart = nullptr;
			fread(pReadCurrent, 1, box.size, pf);

			pMoovStart = pReadCurrent;
			memcpy_b2l(&moov.size, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&moov.type, pReadCurrent, 4); pReadCurrent += 4;

			xbox_moov_mvhd mvhd;
			memcpy_b2l(&mvhd.size, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.type, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.version, pReadCurrent, 1); pReadCurrent += 1;
			memcpy_b2l(mvhd.flags, pReadCurrent, 3); pReadCurrent += 3;
			memcpy_b2l(&mvhd.creationtime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.mofificatiotime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.timescale, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.duration, pReadCurrent, 4); pReadCurrent += 4;


			uint16_t preferredrate_x;
			uint16_t preferredrate_y;

			memcpy_b2l(&preferredrate_x, pReadCurrent, 2); pReadCurrent += 2;
			memcpy_b2l(&preferredrate_y, pReadCurrent, 2); pReadCurrent += 2;

			mvhd.preferredrate = preferredrate_x*1.0 + preferredrate_y / 10.0;

			uint8_t preferredvolume_x;
			uint8_t preferredvolume_y;

			memcpy_b2l(&preferredvolume_x, pReadCurrent, 1); pReadCurrent += 1;
			memcpy_b2l(&preferredvolume_y, pReadCurrent, 1); pReadCurrent += 1;

			mvhd.preferredvolume = preferredvolume_x*1.0 + preferredvolume_y / 10.0;

			memcpy_b2l(mvhd.reserved, pReadCurrent, 10); pReadCurrent += 10;
			for (int i = 0; i < 9; i++)
			{
				memcpy_b2l(&mvhd.matrixstructure[i], pReadCurrent, 4); pReadCurrent += 4;
			}

			memcpy_b2l(&mvhd.previewtime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.previewduration, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.postertime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.selectiontime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.selectionduration, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.currenttime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.nexttrackid, pReadCurrent, 4); pReadCurrent += 4;

			int nTrackIndex = 0;
			while ((pReadCurrent - pMoovStart) < moov.size)
			{
				xbox trak;
				nTrackIndex++;
				memcpy_b2l(&trak.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&trak.type, pReadCurrent, 4); pReadCurrent += 4;

				xbox_moov_trak_tkhd tkhd;
				memcpy_b2l(&tkhd.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.type, pReadCurrent, 4); pReadCurrent += 4;


				memcpy_b2l(&tkhd.version, pReadCurrent, 1); pReadCurrent += 1;
				memcpy_b2l(tkhd.flags, pReadCurrent, 3); pReadCurrent += 3;
				memcpy_b2l(&tkhd.creationtime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.mofificatiotime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.trackid, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(tkhd.reserved1_4, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.duration, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(tkhd.reserved2_8, pReadCurrent, 8); pReadCurrent += 8;
				memcpy_b2l(&tkhd.layer, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&tkhd.alternate_group, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&tkhd.volume, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(tkhd.reserved3_2, pReadCurrent, 2); pReadCurrent += 2;

				for (int i = 0; i < 9; i++)
				{
					memcpy_b2l(&tkhd.matrixstructure[i], pReadCurrent, 4); pReadCurrent += 4;
				}


				uint16_t x;
				uint16_t y;

				memcpy_b2l(&x, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&y, pReadCurrent, 2); pReadCurrent += 2;

				tkhd.track_width = x*1.0 + y / 10.0;

				memcpy_b2l(&x, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&y, pReadCurrent, 2); pReadCurrent += 2;
				tkhd.track_height = x*1.0 + y / 10.0;


				xbox mida;
				memcpy_b2l(&mida.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mida.type, pReadCurrent, 4); pReadCurrent += 4;


				xbox_moov_trak_mdia_mdhd mdhd;

				memcpy_b2l(&mdhd.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.type, pReadCurrent, 4); pReadCurrent += 4;

				memcpy_b2l(&mdhd.version, pReadCurrent, 1); pReadCurrent += 1;
				memcpy_b2l(mdhd.flags, pReadCurrent, 3); pReadCurrent += 3;
				memcpy_b2l(&mdhd.creationtime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.mofificatiotime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.timescale, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.duration, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.language, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&mdhd.quality, pReadCurrent, 2); pReadCurrent += 2;

				xbox_moov_trak_x_hdlr mdia_hdlr;


				memcpy_b2l(&mdia_hdlr.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.type, pReadCurrent, 4); pReadCurrent += 4;

				memcpy_b2l(&mdia_hdlr.version, pReadCurrent, 1); pReadCurrent += 1;
				memcpy_b2l(mdia_hdlr.flags, pReadCurrent, 3); pReadCurrent += 3;
				memcpy_b2l(&mdia_hdlr.componenttype, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentsubtype, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentmaufacturer, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentflags, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentflagsmask, pReadCurrent, 4); pReadCurrent += 4;

				mdia_hdlr.private_componentnamelenght = mdia_hdlr.size - (4 + 4 + 1 + 3 + 4 * 5);

				if (mdia_hdlr.private_componentnamelenght > 0)
				{
					mdia_hdlr.componentname = new char[mdia_hdlr.private_componentnamelenght];
					memcpy(&mdia_hdlr.componentname, pReadCurrent, mdia_hdlr.private_componentnamelenght); pReadCurrent += mdia_hdlr.private_componentnamelenght;
				}

				{
					xbox minf;
					memcpy_b2l(&minf.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf.type, pReadCurrent, 4); pReadCurrent += 4;



					if (mdia_hdlr.componentsubtype == 'soun')
					{
						//struct xbox_moov_trak_mdia_minf_smhd
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint16_t balance;
						//	uint16_t reserved;
						//};

						xbox_moov_trak_mdia_minf_smhd smhd;

						memcpy_b2l(&smhd.size, pReadCurrent, 4); pReadCurrent += 4;


						memcpy_b2l(&smhd.type, pReadCurrent, 4); pReadCurrent += 4;

						memcpy_b2l(&smhd.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(smhd.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&smhd.balance, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&smhd.reserved, pReadCurrent, 2); pReadCurrent += 2;

					}
					else if (mdia_hdlr.componentsubtype == 'vide')
					{
						xbox_moov_trak_mdia_minf_vmhd vmhd;


						memcpy_b2l(&vmhd.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&vmhd.type, pReadCurrent, 4); pReadCurrent += 4;

						memcpy_b2l(&vmhd.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(vmhd.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&vmhd.graphicsmode, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&vmhd.opcolorred, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&vmhd.opcolorgreen, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&vmhd.opcolorblue, pReadCurrent, 2); pReadCurrent += 2;
					}




					xbox_moov_trak_x_hdlr minf_hdlr;


					memcpy_b2l(&minf_hdlr.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.type, pReadCurrent, 4); pReadCurrent += 4;

					memcpy_b2l(&minf_hdlr.version, pReadCurrent, 1); pReadCurrent += 1;
					memcpy_b2l(minf_hdlr.flags, pReadCurrent, 3); pReadCurrent += 3;
					memcpy_b2l(&minf_hdlr.componenttype, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentsubtype, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentmaufacturer, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentflags, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentflagsmask, pReadCurrent, 4); pReadCurrent += 4;

					minf_hdlr.private_componentnamelenght = minf_hdlr.size - (4 + 4 + 1 + 3 + 4 * 5);

					if (minf_hdlr.private_componentnamelenght > 0)
					{
						minf_hdlr.componentname = new char[minf_hdlr.private_componentnamelenght];
						memcpy(&minf_hdlr.componentname, pReadCurrent, minf_hdlr.private_componentnamelenght); pReadCurrent += minf_hdlr.private_componentnamelenght;
					}

					xbox dinf;

					memcpy_b2l(&dinf.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&dinf.type, pReadCurrent, 4); pReadCurrent += 4;

					xbox_moov_trak_mdia_minf_dinf_dref dref;


					memcpy_b2l(&dref.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&dref.type, pReadCurrent, 4); pReadCurrent += 4;

					memcpy_b2l(&dref.version, pReadCurrent, 1); pReadCurrent += 1;
					memcpy_b2l(dref.flags, pReadCurrent, 3); pReadCurrent += 3;
					memcpy_b2l(&dref.number, pReadCurrent, 4); pReadCurrent += 4;

					for (int number = 0; number < dref.number; number++)
					{
						xbox_moov_trak_mdia_minf_dinf_dref_data data;

						memcpy_b2l(&data.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&data.type, pReadCurrent, 4); pReadCurrent += 4;

						memcpy_b2l(&data.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(data.flags, pReadCurrent, 3); pReadCurrent += 3;

						data.private_datasize = data.size - 12;

						if (data.private_datasize > 0)
						{
							data.data = new uint8_t[data.private_datasize];

							memcpy(data.data, pReadCurrent, data.private_datasize); pReadCurrent += data.private_datasize;
						}
					}


					{
						xbox stbl;
						memcpy_b2l(&stbl.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stbl.type, pReadCurrent, 4); pReadCurrent += 4;


						//struct videosampledescription
						//{
						//	uint32_t size;
						//	uint32_t vendor;
						//	uint64_t version;
						//	uint16_t revisionlevel;
						//	uint16_t datarefindex;
						//	uint8_t handler[12];
						//	uint16_t width;
						//	uint16_t height;
						//	char *   description;
						//	uint32_t private_descriptionsize;
						//};

						//struct xbox_moov_trak_mdia_minf_stbl_stsd_for_video
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	videosampledescription *sampledescription;
						//};


						if (mdia_hdlr.componentsubtype == 'soun')
						{

							//struct soundsampledescription
							//{
							//	uint32_t size;
							//	uint32_t vendor;
							//	uint16_t datarefindex;
							//	uint64_t version;
							//	uint16_t revisionlevel;
							//	uint32_t numberofchannels;
							//	uint16_t siamplesize;
							//	uint16_t compressionid;
							//	uint16_t packetsize;
							//	uint32_t samplerate;
							//};

							//struct xbox_moov_trak_mdia_minf_stbl_stsd_for_sound
							//{
							//	uint32_t size;
							//	uint32_t type;
							//	uint8_t version;
							//	uint8_t flags[3];
							//	uint32_t number;
							//	soundsampledescription *sampledescription;
							//};

							xbox_moov_trak_mdia_minf_stbl_stsd_for_sound stsd;
							memcpy_b2l(&stsd.size, pReadCurrent, 4); pReadCurrent += 4;

							pReadCurrent += stsd.size - 4;

							//now i dont know at all about this sound description.so skip it.

							//memcpy_b2l(&stsd.type, pReadCurrent, 4); pReadCurrent += 4;
							//memcpy_b2l(&stsd.version, pReadCurrent, 1); pReadCurrent += 1;
							//memcpy_b2l(stsd.flags, pReadCurrent, 3); pReadCurrent += 3;
							//memcpy_b2l(&stsd.number, pReadCurrent, 4); pReadCurrent += 4;

							//stsd.sampledescription = new soundsampledescription[stsd.number];

							//for (int stsdnumber = 0; stsdnumber < stsd.number; stsdnumber++)
							//{
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].size, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].vendor, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].datarefindex, pReadCurrent, 2); pReadCurrent += 2;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].version, pReadCurrent, 2); pReadCurrent += 2;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].revisionlevel, pReadCurrent, 2); pReadCurrent += 2;

							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].numberofchannels, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].siamplesize, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].compressionid, pReadCurrent, 8); pReadCurrent += 8;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].packetsize, pReadCurrent, 2); pReadCurrent += 2;
							//	//memcpy_b2l(&stsd.sampledescription[stsdnumber].height, pReadCurrent, 2); pReadCurrent += 2;

							//	//stsd.sampledescription[stsdnumber].private_descriptionsize = stsd.sampledescription[stsdnumber].size - 4 - 4 - 8 - 2 - 2 - 12 - 2 - 2;
							//	//stsd.sampledescription[stsdnumber].description = new uint8_t[stsd.sampledescription[stsdnumber].private_descriptionsize];

							//	//memcpy(stsd.sampledescription[stsdnumber].description, pReadCurrent, stsd.sampledescription[stsdnumber].private_descriptionsize); pReadCurrent += stsd.sampledescription[stsdnumber].private_descriptionsize;

							//}
						}
						else if (mdia_hdlr.componentsubtype == 'vide')
						{
							xbox_moov_trak_mdia_minf_stbl_stsd_for_video stsd;
							memcpy_b2l(&stsd.size, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsd.type, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsd.version, pReadCurrent, 1); pReadCurrent += 1;
							memcpy_b2l(stsd.flags, pReadCurrent, 3); pReadCurrent += 3;
							memcpy_b2l(&stsd.number, pReadCurrent, 4); pReadCurrent += 4;

							stsd.sampledescription = new videosampledescription[stsd.number];

							for (int stsdnumber = 0; stsdnumber < stsd.number; stsdnumber++)
							{
								memcpy_b2l(&stsd.sampledescription[stsdnumber].size, pReadCurrent, 4); pReadCurrent += 4;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].vendor, pReadCurrent, 4); pReadCurrent += 4;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].version, pReadCurrent, 8); pReadCurrent += 8;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].revisionlevel, pReadCurrent, 2); pReadCurrent += 2;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].datarefindex, pReadCurrent, 2); pReadCurrent += 2;
								memcpy(stsd.sampledescription[stsdnumber].handler, pReadCurrent, 12); pReadCurrent += 12;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].width, pReadCurrent, 2); pReadCurrent += 2;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].height, pReadCurrent, 2); pReadCurrent += 2;

								stsd.sampledescription[stsdnumber].private_descriptionsize = stsd.sampledescription[stsdnumber].size - 4 - 4 - 8 - 2 - 2 - 12 - 2 - 2;
								stsd.sampledescription[stsdnumber].description = new uint8_t[stsd.sampledescription[stsdnumber].private_descriptionsize];

								memcpy(stsd.sampledescription[stsdnumber].description, pReadCurrent, stsd.sampledescription[stsdnumber].private_descriptionsize); pReadCurrent += stsd.sampledescription[stsdnumber].private_descriptionsize;

							}

						}



						//struct time_to_sample
						//{
						//	uint32_t samplecount;
						//	uint32_t sampleduration;
						//};
						//struct xbox_moov_trak_mdia_minf_stbl_stts //time to sample
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	time_to_sample * timetosample;
						//};

						xbox_moov_trak_mdia_minf_stbl_stts stts;
						memcpy_b2l(&stts.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stts.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stts.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(stts.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&stts.number, pReadCurrent, 4); pReadCurrent += 4;

						stts.timetosample = new time_to_sample[stts.number];

						for (int timetosampleindex = 0; timetosampleindex < stts.number; timetosampleindex++)
						{
							memcpy_b2l(&stts.timetosample[timetosampleindex].samplecount, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stts.timetosample[timetosampleindex].sampleduration, pReadCurrent, 4); pReadCurrent += 4;
						}



						//struct sample_to_chunk_table
						//{
						//	uint32_t firstchunk;
						//	uint32_t samplesperchunk;
						//	uint32_t sampledescriptionid;
						//};

						//struct xbox_moov_trak_mdia_minf_stbl_stsc //sample to chunk
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	sample_to_chunk_table * sampletochunk;
						//};

						xbox_moov_trak_mdia_minf_stbl_stsc stsc;
						memcpy_b2l(&stsc.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsc.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsc.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(stsc.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&stsc.number, pReadCurrent, 4); pReadCurrent += 4;

						stsc.sampletochunk = new sample_to_chunk_table[stsc.number];

						for (int sampletochunkindex = 0; sampletochunkindex < stsc.number; sampletochunkindex++)
						{
							memcpy_b2l(&stsc.sampletochunk[sampletochunkindex].firstchunk, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsc.sampletochunk[sampletochunkindex].samplesperchunk, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsc.sampletochunk[sampletochunkindex].sampledescriptionid, pReadCurrent, 4); pReadCurrent += 4;
						}

						//test code
						//uint32_t sampletotal = 0;

						//for (int sampletochunkindex = 0; sampletochunkindex < stsc.number; sampletochunkindex++)
						//{
						//	if (sampletochunkindex > 0)
						//	{
						//		int chunks = stsc.sampletochunk[sampletochunkindex].firstchunk - stsc.sampletochunk[sampletochunkindex - 1].firstchunk;
						//		sampletotal += chunks*stsc.sampletochunk[sampletochunkindex - 1].samplesperchunk;
						//	}

						//}

						//uint32_t t = sampletotal + 1 - 1;



						//struct xbox_moov_trak_mdia_minf_stbl_stsz //sample to chunk
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t samplesize;
						//	uint32_t number;
						//	uint32_t * samplesizetable;
						//};

						xbox_moov_trak_mdia_minf_stbl_stsz stsz;

						memcpy_b2l(&stsz.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsz.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsz.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(stsz.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&stsz.samplesize, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsz.number, pReadCurrent, 4); pReadCurrent += 4;

						if ((stsz.samplesize == 0) && (stsz.number > 0))
						{
							stsz.samplesizetable = new uint32_t[stsz.number];

							for (int sampleindex = 0; sampleindex < stsz.number; sampleindex++)
							{
								memcpy_b2l(&stsz.samplesizetable[sampleindex], pReadCurrent, 4); pReadCurrent += 4;
							}
						}


						//struct xbox_moov_trak_mdia_minf_stbl_co64 //chunk offset
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	uint64_t *chunkoffsettable;
						//};


						xbox_moov_trak_mdia_minf_stbl_co64 co64;

						memcpy_b2l(&co64.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&co64.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&co64.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(co64.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&co64.number, pReadCurrent, 4); pReadCurrent += 4;

						if (co64.number > 0)
						{
							co64.chunkoffsettable = new uint64_t[co64.number];

							for (int chunkindex = 0; chunkindex < co64.number; chunkindex++)
							{
								memcpy_b2l(&co64.chunkoffsettable[chunkindex], pReadCurrent, 8); pReadCurrent += 8;
							}
						}

						// i dont know at all about snts, so skip it.


						if (mdia_hdlr.componentsubtype == 'vide')
						{
							xbox snts;
							memcpy_b2l(&snts.size, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&snts.type, pReadCurrent, 4); pReadCurrent += 4;

							pReadCurrent += snts.size - 8;
						}

					}


				}

			}

			ui64CurrentReadPos += box.size;//seek plus moov box size.
		}

		if (box.type == 'mdat')
		{
			ui64CurrentReadPos += box.size;//seek plus moov box size.
		}

	}

	if (pf != nullptr)
	{
		fclose(pf);
	}

	if (pReadCurrent != nullptr)
	{
		delete[] pReadCurrent;
	}
}

int x_mov_open(x_mov_file *file, char *pszfilename)
{
	FILE *pf = nullptr;

	//fopen_s(&pf, "T:\\footage\\UHD\\20170424165112\\High\\节目01_20170424165112_1.mov", "rb");
	errno_t err = fopen_s(&pf, pszfilename, "rb");

	if (err != 0)
	{
		return err;
	}
	file->pf = pf;


	_fseeki64(pf, 0, SEEK_END);

	uint64_t ui64FileSize = _ftelli64(pf);
	char *pReadCurrent;
	uint64_t ui64CurrentReadPos = 0;

	uint64_t ui64lastreadbufferlength = 1024;

	pReadCurrent = new char[ui64lastreadbufferlength];

	while (ui64CurrentReadPos < ui64FileSize)
	{
		_fseeki64(pf, ui64CurrentReadPos, SEEK_SET);

		fread(pReadCurrent, 1, 8, pf);

		xbox box;

		memcpy_b2l(&box.size, pReadCurrent, 4);

		memcpy_b2l(&box.type, pReadCurrent + 4, 4);


		if (box.type == 'ftyp')
		{
			_fseeki64(pf, ui64CurrentReadPos, SEEK_SET);

			if (box.size > ui64lastreadbufferlength)
			{
				delete[]pReadCurrent;

				pReadCurrent = new char[box.size];

				ui64lastreadbufferlength = box.size;
			}

			fread(pReadCurrent, 1, box.size, pf);

			xbox_file_type ft;

			memcpy_b2l(&ft.size, pReadCurrent, 4);

			memcpy_b2l(&ft.type, pReadCurrent + 4, 4);

			memcpy_b2l(&ft.major_brand, pReadCurrent + 8, 4);

			memcpy_b2l(&ft.minor_version, pReadCurrent + 12, 4);

			ft.compatible_brands_count = (ft.size - 16) / 4;
			ft.compatible_brands = new uint32_t*[ft.compatible_brands_count];

			for (auto i = 0; i < ft.compatible_brands_count; i++)
			{
				ft.compatible_brands[i] = new uint32_t;

				memcpy_b2l(ft.compatible_brands[i], pReadCurrent + 16 + i * 4, 4);
			}

			if (*ft.compatible_brands[0] == 'qt  ')
			{
				//it's quick time file standard.
			}

			ui64CurrentReadPos += box.size;
		}

		if (box.type == 'free')
		{
			ui64CurrentReadPos += box.size;
		}

		if (box.type == 'skip')
		{
			ui64CurrentReadPos += box.size;
		}

		if (box.type == 'moov')
		{
			xbox moov;

			_fseeki64(pf, ui64CurrentReadPos, SEEK_SET);

			if (box.size > ui64lastreadbufferlength)
			{
				delete[]pReadCurrent;

				pReadCurrent = new char[box.size];

				ui64lastreadbufferlength = box.size;
			}

			char * pMoovStart = nullptr;
			fread(pReadCurrent, 1, box.size, pf);

			pMoovStart = pReadCurrent;
			memcpy_b2l(&moov.size, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&moov.type, pReadCurrent, 4); pReadCurrent += 4;

			xbox_moov_mvhd mvhd;
			memcpy_b2l(&mvhd.size, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.type, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.version, pReadCurrent, 1); pReadCurrent += 1;
			memcpy_b2l(mvhd.flags, pReadCurrent, 3); pReadCurrent += 3;
			memcpy_b2l(&mvhd.creationtime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.mofificatiotime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.timescale, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.duration, pReadCurrent, 4); pReadCurrent += 4;

			file->duration = mvhd.duration;
			file->timescale = mvhd.timescale;

			uint16_t preferredrate_x;
			uint16_t preferredrate_y;

			memcpy_b2l(&preferredrate_x, pReadCurrent, 2); pReadCurrent += 2;
			memcpy_b2l(&preferredrate_y, pReadCurrent, 2); pReadCurrent += 2;

			mvhd.preferredrate = preferredrate_x*1.0 + preferredrate_y / 10.0;

			uint8_t preferredvolume_x;
			uint8_t preferredvolume_y;

			memcpy_b2l(&preferredvolume_x, pReadCurrent, 1); pReadCurrent += 1;
			memcpy_b2l(&preferredvolume_y, pReadCurrent, 1); pReadCurrent += 1;

			mvhd.preferredvolume = preferredvolume_x*1.0 + preferredvolume_y / 10.0;

			memcpy_b2l(mvhd.reserved, pReadCurrent, 10); pReadCurrent += 10;
			for (int i = 0; i < 9; i++)
			{
				memcpy_b2l(&mvhd.matrixstructure[i], pReadCurrent, 4); pReadCurrent += 4;
			}

			memcpy_b2l(&mvhd.previewtime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.previewduration, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.postertime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.selectiontime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.selectionduration, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.currenttime, pReadCurrent, 4); pReadCurrent += 4;
			memcpy_b2l(&mvhd.nexttrackid, pReadCurrent, 4); pReadCurrent += 4;

			int nAudioTrackIndex = 0;
			bool bvedio = false;
			while ((pReadCurrent - pMoovStart) < moov.size)
			{
				xbox trak;
		
				memcpy_b2l(&trak.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&trak.type, pReadCurrent, 4); pReadCurrent += 4;

				xbox_moov_trak_tkhd tkhd;
				memcpy_b2l(&tkhd.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.type, pReadCurrent, 4); pReadCurrent += 4;


				memcpy_b2l(&tkhd.version, pReadCurrent, 1); pReadCurrent += 1;
				memcpy_b2l(tkhd.flags, pReadCurrent, 3); pReadCurrent += 3;
				memcpy_b2l(&tkhd.creationtime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.mofificatiotime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.trackid, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(tkhd.reserved1_4, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&tkhd.duration, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(tkhd.reserved2_8, pReadCurrent, 8); pReadCurrent += 8;
				memcpy_b2l(&tkhd.layer, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&tkhd.alternate_group, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&tkhd.volume, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(tkhd.reserved3_2, pReadCurrent, 2); pReadCurrent += 2;

				for (int i = 0; i < 9; i++)
				{
					memcpy_b2l(&tkhd.matrixstructure[i], pReadCurrent, 4); pReadCurrent += 4;
				}


				uint16_t x;
				uint16_t y;

				memcpy_b2l(&x, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&y, pReadCurrent, 2); pReadCurrent += 2;

				tkhd.track_width = x*1.0 + y / 10.0;

				memcpy_b2l(&x, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&y, pReadCurrent, 2); pReadCurrent += 2;
				tkhd.track_height = x*1.0 + y / 10.0;


				xbox mida;
				memcpy_b2l(&mida.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mida.type, pReadCurrent, 4); pReadCurrent += 4;


				xbox_moov_trak_mdia_mdhd mdhd;

				memcpy_b2l(&mdhd.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.type, pReadCurrent, 4); pReadCurrent += 4;

				memcpy_b2l(&mdhd.version, pReadCurrent, 1); pReadCurrent += 1;
				memcpy_b2l(mdhd.flags, pReadCurrent, 3); pReadCurrent += 3;
				memcpy_b2l(&mdhd.creationtime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.mofificatiotime, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.timescale, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.duration, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdhd.language, pReadCurrent, 2); pReadCurrent += 2;
				memcpy_b2l(&mdhd.quality, pReadCurrent, 2); pReadCurrent += 2;

				xbox_moov_trak_x_hdlr mdia_hdlr;


				memcpy_b2l(&mdia_hdlr.size, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.type, pReadCurrent, 4); pReadCurrent += 4;

				memcpy_b2l(&mdia_hdlr.version, pReadCurrent, 1); pReadCurrent += 1;
				memcpy_b2l(mdia_hdlr.flags, pReadCurrent, 3); pReadCurrent += 3;
				memcpy_b2l(&mdia_hdlr.componenttype, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentsubtype, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentmaufacturer, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentflags, pReadCurrent, 4); pReadCurrent += 4;
				memcpy_b2l(&mdia_hdlr.componentflagsmask, pReadCurrent, 4); pReadCurrent += 4;

				mdia_hdlr.private_componentnamelenght = mdia_hdlr.size - (4 + 4 + 1 + 3 + 4 * 5);

				if (mdia_hdlr.private_componentnamelenght > 0)
				{
					mdia_hdlr.componentname = new char[mdia_hdlr.private_componentnamelenght];
					memcpy(&mdia_hdlr.componentname, pReadCurrent, mdia_hdlr.private_componentnamelenght); pReadCurrent += mdia_hdlr.private_componentnamelenght;
				}

				if (mdia_hdlr.componentsubtype == 'vide')
				{
					bvedio = true;
				}
				else
				{
					bvedio = false;
					nAudioTrackIndex++;
				}

				if (bvedio)
				{
					file->vtrack.tkhd = tkhd;
				}
				else
				{
					file->atrack[nAudioTrackIndex - 1].tkhd = tkhd;
				}

			
				{
					xbox minf;
					memcpy_b2l(&minf.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf.type, pReadCurrent, 4); pReadCurrent += 4;




					if (mdia_hdlr.componentsubtype == 'soun')
					{
						//struct xbox_moov_trak_mdia_minf_smhd
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint16_t balance;
						//	uint16_t reserved;
						//};

						xbox_moov_trak_mdia_minf_smhd smhd;

						memcpy_b2l(&smhd.size, pReadCurrent, 4); pReadCurrent += 4;


						memcpy_b2l(&smhd.type, pReadCurrent, 4); pReadCurrent += 4;

						memcpy_b2l(&smhd.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(smhd.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&smhd.balance, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&smhd.reserved, pReadCurrent, 2); pReadCurrent += 2;

					}
					else if (mdia_hdlr.componentsubtype == 'vide')
					{
						xbox_moov_trak_mdia_minf_vmhd vmhd;


						memcpy_b2l(&vmhd.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&vmhd.type, pReadCurrent, 4); pReadCurrent += 4;

						memcpy_b2l(&vmhd.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(vmhd.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&vmhd.graphicsmode, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&vmhd.opcolorred, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&vmhd.opcolorgreen, pReadCurrent, 2); pReadCurrent += 2;
						memcpy_b2l(&vmhd.opcolorblue, pReadCurrent, 2); pReadCurrent += 2;
					}




					xbox_moov_trak_x_hdlr minf_hdlr;


					memcpy_b2l(&minf_hdlr.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.type, pReadCurrent, 4); pReadCurrent += 4;

					memcpy_b2l(&minf_hdlr.version, pReadCurrent, 1); pReadCurrent += 1;
					memcpy_b2l(minf_hdlr.flags, pReadCurrent, 3); pReadCurrent += 3;
					memcpy_b2l(&minf_hdlr.componenttype, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentsubtype, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentmaufacturer, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentflags, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&minf_hdlr.componentflagsmask, pReadCurrent, 4); pReadCurrent += 4;

					minf_hdlr.private_componentnamelenght = minf_hdlr.size - (4 + 4 + 1 + 3 + 4 * 5);

					if (minf_hdlr.private_componentnamelenght > 0)
					{
						minf_hdlr.componentname = new char[minf_hdlr.private_componentnamelenght];
						memcpy(&minf_hdlr.componentname, pReadCurrent, minf_hdlr.private_componentnamelenght); pReadCurrent += minf_hdlr.private_componentnamelenght;
					}

					xbox dinf;

					memcpy_b2l(&dinf.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&dinf.type, pReadCurrent, 4); pReadCurrent += 4;

					xbox_moov_trak_mdia_minf_dinf_dref dref;


					memcpy_b2l(&dref.size, pReadCurrent, 4); pReadCurrent += 4;
					memcpy_b2l(&dref.type, pReadCurrent, 4); pReadCurrent += 4;

					memcpy_b2l(&dref.version, pReadCurrent, 1); pReadCurrent += 1;
					memcpy_b2l(dref.flags, pReadCurrent, 3); pReadCurrent += 3;
					memcpy_b2l(&dref.number, pReadCurrent, 4); pReadCurrent += 4;

					for (int number = 0; number < dref.number; number++)
					{
						xbox_moov_trak_mdia_minf_dinf_dref_data data;

						memcpy_b2l(&data.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&data.type, pReadCurrent, 4); pReadCurrent += 4;

						memcpy_b2l(&data.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(data.flags, pReadCurrent, 3); pReadCurrent += 3;

						data.private_datasize = data.size - 12;

						if (data.private_datasize > 0)
						{
							data.data = new uint8_t[data.private_datasize];

							memcpy(data.data, pReadCurrent, data.private_datasize); pReadCurrent += data.private_datasize;
						}
					}


					{
						xbox stbl;
						memcpy_b2l(&stbl.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stbl.type, pReadCurrent, 4); pReadCurrent += 4;


						//struct videosampledescription
						//{
						//	uint32_t size;
						//	uint32_t vendor;
						//	uint64_t version;
						//	uint16_t revisionlevel;
						//	uint16_t datarefindex;
						//	uint8_t handler[12];
						//	uint16_t width;
						//	uint16_t height;
						//	char *   description;
						//	uint32_t private_descriptionsize;
						//};

						//struct xbox_moov_trak_mdia_minf_stbl_stsd_for_video
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	videosampledescription *sampledescription;
						//};


						if (mdia_hdlr.componentsubtype == 'soun')
						{

							//struct soundsampledescription
							//{
							//	uint32_t size;
							//	uint32_t vendor;
							//	uint16_t datarefindex;
							//	uint64_t version;
							//	uint16_t revisionlevel;
							//	uint32_t numberofchannels;
							//	uint16_t siamplesize;
							//	uint16_t compressionid;
							//	uint16_t packetsize;
							//	uint32_t samplerate;
							//};

							//struct xbox_moov_trak_mdia_minf_stbl_stsd_for_sound
							//{
							//	uint32_t size;
							//	uint32_t type;
							//	uint8_t version;
							//	uint8_t flags[3];
							//	uint32_t number;
							//	soundsampledescription *sampledescription;
							//};

							xbox_moov_trak_mdia_minf_stbl_stsd_for_sound stsd;
							memcpy_b2l(&stsd.size, pReadCurrent, 4); pReadCurrent += 4;

							pReadCurrent += stsd.size - 4;

							//now i dont know at all about this sound description.so skip it.

							//memcpy_b2l(&stsd.type, pReadCurrent, 4); pReadCurrent += 4;
							//memcpy_b2l(&stsd.version, pReadCurrent, 1); pReadCurrent += 1;
							//memcpy_b2l(stsd.flags, pReadCurrent, 3); pReadCurrent += 3;
							//memcpy_b2l(&stsd.number, pReadCurrent, 4); pReadCurrent += 4;

							//stsd.sampledescription = new soundsampledescription[stsd.number];

							//for (int stsdnumber = 0; stsdnumber < stsd.number; stsdnumber++)
							//{
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].size, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].vendor, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].datarefindex, pReadCurrent, 2); pReadCurrent += 2;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].version, pReadCurrent, 2); pReadCurrent += 2;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].revisionlevel, pReadCurrent, 2); pReadCurrent += 2;

							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].numberofchannels, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].siamplesize, pReadCurrent, 4); pReadCurrent += 4;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].compressionid, pReadCurrent, 8); pReadCurrent += 8;
							//	memcpy_b2l(&stsd.sampledescription[stsdnumber].packetsize, pReadCurrent, 2); pReadCurrent += 2;
							//	//memcpy_b2l(&stsd.sampledescription[stsdnumber].height, pReadCurrent, 2); pReadCurrent += 2;

							//	//stsd.sampledescription[stsdnumber].private_descriptionsize = stsd.sampledescription[stsdnumber].size - 4 - 4 - 8 - 2 - 2 - 12 - 2 - 2;
							//	//stsd.sampledescription[stsdnumber].description = new uint8_t[stsd.sampledescription[stsdnumber].private_descriptionsize];

							//	//memcpy(stsd.sampledescription[stsdnumber].description, pReadCurrent, stsd.sampledescription[stsdnumber].private_descriptionsize); pReadCurrent += stsd.sampledescription[stsdnumber].private_descriptionsize;

							//}
						}
						else if (mdia_hdlr.componentsubtype == 'vide')
						{
							xbox_moov_trak_mdia_minf_stbl_stsd_for_video stsd;
							memcpy_b2l(&stsd.size, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsd.type, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsd.version, pReadCurrent, 1); pReadCurrent += 1;
							memcpy_b2l(stsd.flags, pReadCurrent, 3); pReadCurrent += 3;
							memcpy_b2l(&stsd.number, pReadCurrent, 4); pReadCurrent += 4;

							stsd.sampledescription = new videosampledescription[stsd.number];

							for (int stsdnumber = 0; stsdnumber < stsd.number; stsdnumber++)
							{
								memcpy_b2l(&stsd.sampledescription[stsdnumber].size, pReadCurrent, 4); pReadCurrent += 4;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].vendor, pReadCurrent, 4); pReadCurrent += 4;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].version, pReadCurrent, 8); pReadCurrent += 8;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].revisionlevel, pReadCurrent, 2); pReadCurrent += 2;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].datarefindex, pReadCurrent, 2); pReadCurrent += 2;
								memcpy(stsd.sampledescription[stsdnumber].handler, pReadCurrent, 12); pReadCurrent += 12;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].width, pReadCurrent, 2); pReadCurrent += 2;
								memcpy_b2l(&stsd.sampledescription[stsdnumber].height, pReadCurrent, 2); pReadCurrent += 2;

								stsd.sampledescription[stsdnumber].private_descriptionsize = stsd.sampledescription[stsdnumber].size - 4 - 4 - 8 - 2 - 2 - 12 - 2 - 2;
								stsd.sampledescription[stsdnumber].description = new uint8_t[stsd.sampledescription[stsdnumber].private_descriptionsize];

								memcpy(stsd.sampledescription[stsdnumber].description, pReadCurrent, stsd.sampledescription[stsdnumber].private_descriptionsize); pReadCurrent += stsd.sampledescription[stsdnumber].private_descriptionsize;

							}

						}



						//struct time_to_sample
						//{
						//	uint32_t samplecount;
						//	uint32_t sampleduration;
						//};
						//struct xbox_moov_trak_mdia_minf_stbl_stts //time to sample
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	time_to_sample * timetosample;
						//};

						xbox_moov_trak_mdia_minf_stbl_stts stts;
						memcpy_b2l(&stts.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stts.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stts.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(stts.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&stts.number, pReadCurrent, 4); pReadCurrent += 4;

						stts.timetosample = new time_to_sample[stts.number];

						for (int timetosampleindex = 0; timetosampleindex < stts.number; timetosampleindex++)
						{
							memcpy_b2l(&stts.timetosample[timetosampleindex].samplecount, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stts.timetosample[timetosampleindex].sampleduration, pReadCurrent, 4); pReadCurrent += 4;
						}



						if (bvedio)
						{
							file->vtrack.stts = stts;
						}
						else
						{
							file->atrack[nAudioTrackIndex - 1].stts = stts;
						}



						//struct sample_to_chunk_table
						//{
						//	uint32_t firstchunk;
						//	uint32_t samplesperchunk;
						//	uint32_t sampledescriptionid;
						//};

						//struct xbox_moov_trak_mdia_minf_stbl_stsc //sample to chunk
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	sample_to_chunk_table * sampletochunk;
						//};

						xbox_moov_trak_mdia_minf_stbl_stsc stsc;
						memcpy_b2l(&stsc.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsc.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsc.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(stsc.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&stsc.number, pReadCurrent, 4); pReadCurrent += 4;

						stsc.sampletochunk = new sample_to_chunk_table[stsc.number];

						for (int sampletochunkindex = 0; sampletochunkindex < stsc.number; sampletochunkindex++)
						{
							memcpy_b2l(&stsc.sampletochunk[sampletochunkindex].firstchunk, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsc.sampletochunk[sampletochunkindex].samplesperchunk, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&stsc.sampletochunk[sampletochunkindex].sampledescriptionid, pReadCurrent, 4); pReadCurrent += 4;
						}

						if (bvedio)
						{
							file->vtrack.stsc = stsc;
						}
						else
						{
							file->atrack[nAudioTrackIndex - 1].stsc = stsc;
						}


						//test code
						//uint32_t sampletotal = 0;

						//for (int sampletochunkindex = 0; sampletochunkindex < stsc.number; sampletochunkindex++)
						//{
						//	if (sampletochunkindex > 0)
						//	{
						//		int chunks = stsc.sampletochunk[sampletochunkindex].firstchunk - stsc.sampletochunk[sampletochunkindex - 1].firstchunk;
						//		sampletotal += chunks*stsc.sampletochunk[sampletochunkindex - 1].samplesperchunk;
						//	}

						//}

						//uint32_t t = sampletotal + 1 - 1;



						//struct xbox_moov_trak_mdia_minf_stbl_stsz //sample to chunk
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t samplesize;
						//	uint32_t number;
						//	uint32_t * samplesizetable;
						//};

						xbox_moov_trak_mdia_minf_stbl_stsz stsz;

						memcpy_b2l(&stsz.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsz.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsz.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(stsz.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&stsz.samplesize, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&stsz.number, pReadCurrent, 4); pReadCurrent += 4;

						if ((stsz.samplesize == 0) && (stsz.number > 0))
						{
							stsz.samplesizetable = new uint32_t[stsz.number];

							for (int sampleindex = 0; sampleindex < stsz.number; sampleindex++)
							{
								memcpy_b2l(&stsz.samplesizetable[sampleindex], pReadCurrent, 4); pReadCurrent += 4;
							}
						}

						if (bvedio)
						{
							file->vtrack.stsz = stsz;
						}
						else
						{
							file->atrack[nAudioTrackIndex - 1].stsz = stsz;
						}


						//struct xbox_moov_trak_mdia_minf_stbl_co64 //chunk offset
						//{
						//	uint32_t size;
						//	uint32_t type;
						//	uint8_t version;
						//	uint8_t flags[3];
						//	uint32_t number;
						//	uint64_t *chunkoffsettable;
						//};


						xbox_moov_trak_mdia_minf_stbl_co64 co64;

						memcpy_b2l(&co64.size, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&co64.type, pReadCurrent, 4); pReadCurrent += 4;
						memcpy_b2l(&co64.version, pReadCurrent, 1); pReadCurrent += 1;
						memcpy_b2l(co64.flags, pReadCurrent, 3); pReadCurrent += 3;
						memcpy_b2l(&co64.number, pReadCurrent, 4); pReadCurrent += 4;

						if (co64.number > 0)
						{
							co64.chunkoffsettable = new uint64_t[co64.number];

							for (int chunkindex = 0; chunkindex < co64.number; chunkindex++)
							{
								memcpy_b2l(&co64.chunkoffsettable[chunkindex], pReadCurrent, 8); pReadCurrent += 8;
							}
						}

						if (bvedio)
						{
							file->vtrack.co64 = co64;
						}
						else
						{
							file->atrack[nAudioTrackIndex - 1].co64 = co64;
						}


						// i dont know at all about snts, so skip it.


						if (mdia_hdlr.componentsubtype == 'vide')
						{
							xbox snts;
							memcpy_b2l(&snts.size, pReadCurrent, 4); pReadCurrent += 4;
							memcpy_b2l(&snts.type, pReadCurrent, 4); pReadCurrent += 4;

							pReadCurrent += snts.size - 8;
						}

					}


				}

			}

			ui64CurrentReadPos += box.size;//seek plus moov box size.
		}

		if (box.type == 'mdat')
		{
			ui64CurrentReadPos += box.size;//seek plus moov box size.
		}

	}

	if (pf != nullptr)
	{
		fclose(pf);
	}

	//if (pReadCurrent != nullptr)
	//{
	//	delete pReadCurrent;
	//}
}


int x_mov_get_video_package(x_mov_file *file, uint64_t ui64frameindex, uint64_t *out_pui64offset, uint64_t*out_pui64size)
{
	uint64_t ui64offset = 0;
	uint64_t ui64size = 0;

	if (ui64frameindex > file->vtrack.stsz.number)
	{
		return 1;
	}

	ui64size = file->vtrack.stsz.samplesizetable[ui64frameindex];

	//find chunk index,wich contains this frame
	uint64_t ui64ChunkIndex = 0;
	uint64_t ui64FisrtFrameIndexOfChunk = 0;

	uint64_t ui64FrameCount = 0;

	uint64_t ui64PreChunkGroupLastFrameCount = 0;
	int64_t i64LastChunkFrameCount = 0;
	for (uint64_t index = 0; index < file->vtrack.stsc.number; index++)
	{
		
		if ((index + 1) < file->vtrack.stsc.number)
		{
			ui64FrameCount += (file->vtrack.stsc.sampletochunk[index + 1].firstchunk - file->vtrack.stsc.sampletochunk[index].firstchunk)*file->vtrack.stsc.sampletochunk[index].samplesperchunk;
		}
		else
		{//如果在最后一个chunk中
			ui64FrameCount += file->vtrack.stsc.sampletochunk[index].samplesperchunk;
		}

		if (ui64frameindex < ui64FrameCount)
		{
			ui64ChunkIndex = file->vtrack.stsc.sampletochunk[index].firstchunk - 1;


			//ui64LastChunkFrameCount = ui64PreChunkGroupLastFrameCount + ( (ui64frameindex - ui64PreChunkGroupLastFrameIndex) / file->vtrack.stsc.sampletochunk[index].samplesperchunk )*
			uint64_t ui64SkipChunks = (ui64frameindex - ui64PreChunkGroupLastFrameCount) / file->vtrack.stsc.sampletochunk[index].samplesperchunk;

			ui64ChunkIndex += ui64SkipChunks;
			//计算包换此frame的上一个chunk的最后一帧在整个文件中是第多少帧，据此后面可以计算出，此frame在文中的offset。
			//计算offset方法：
			//1得到包含此frame的chunk在文件中的offset；
			//2计算此chunk中，此frame前所有帧的size总和。
			//1+2，即为此frame在文件中的offset。
			i64LastChunkFrameCount = ui64PreChunkGroupLastFrameCount + ui64SkipChunks* file->vtrack.stsc.sampletochunk[index].samplesperchunk -1;

			
			break;
		}
		ui64PreChunkGroupLastFrameCount = ui64FrameCount;
	}


	//find this chunk offset
	ui64offset = file->vtrack.co64.chunkoffsettable[ui64ChunkIndex];


	////calc this current frame offset
	for (uint64_t index = i64LastChunkFrameCount+1; index < ui64frameindex; index++)
	{
		ui64offset += file->vtrack.stsz.samplesizetable[index];
	}
	
	return 0;
}

int x_mov_get_audio_package(x_mov_file *file, uint64_t ui64frameindex, uint32_t ui32channelindex, uint64_t *out_pui64offset, uint64_t*out_pui64size)
{
	uint64_t ui64offset = 0;
	uint64_t ui64size = 0;

	if (ui64frameindex > file->atrack[ui32channelindex].stsz.number)
	{
		return 1;
	}

	//ui64size = file->atrack[ui32channelindex].stsz.samplesizetable[ui64frameindex];

	ui64size = (48000 / 50) * 2;


	//find chunk index,wich contains this frame
	uint64_t ui64ChunkIndex = 0;
	uint64_t ui64FisrtFrameIndexOfChunk = 0;

	uint64_t ui64FrameCount = 0;

	uint64_t ui64PreChunkGroupLastFrameCount = 0;
	int64_t i64LastChunkFrameCount = 0;
	for (uint64_t index = 0; index < file->atrack[ui32channelindex].stsc.number; index++)
	{

		if ((index + 1) < file->atrack[ui32channelindex].stsc.number)
		{
			ui64FrameCount += (file->atrack[ui32channelindex].stsc.sampletochunk[index + 1].firstchunk - file->atrack[ui32channelindex].stsc.sampletochunk[index].firstchunk)*file->atrack[ui32channelindex].stsc.sampletochunk[index].samplesperchunk;
		}
		else
		{//如果在最后一个chunk中
			ui64FrameCount += file->atrack[ui32channelindex].stsc.sampletochunk[index].samplesperchunk;
		}

		if (ui64frameindex < ui64FrameCount)
		{
			ui64ChunkIndex = file->atrack[ui32channelindex].stsc.sampletochunk[index].firstchunk - 1;


			//ui64LastChunkFrameCount = ui64PreChunkGroupLastFrameCount + ( (ui64frameindex - ui64PreChunkGroupLastFrameIndex) / file->vtrack.stsc.sampletochunk[index].samplesperchunk )*
			uint64_t ui64SkipChunks = (ui64frameindex - ui64PreChunkGroupLastFrameCount) / file->atrack[ui32channelindex].stsc.sampletochunk[index].samplesperchunk;

			ui64ChunkIndex += ui64SkipChunks;
			//计算包换此frame的上一个chunk的最后一帧在整个文件中是第多少帧，据此后面可以计算出，此frame在文中的offset。
			//计算offset方法：
			//1得到包含此frame的chunk在文件中的offset；
			//2计算此chunk中，此frame前所有帧的size总和。
			//1+2，即为此frame在文件中的offset。
			i64LastChunkFrameCount = ui64PreChunkGroupLastFrameCount + ui64SkipChunks* file->atrack[ui32channelindex].stsc.sampletochunk[index].samplesperchunk - 1;


			break;
		}
		ui64PreChunkGroupLastFrameCount = ui64FrameCount;
	}


	//find this chunk offset
	ui64offset = file->atrack[ui32channelindex].co64.chunkoffsettable[ui64ChunkIndex];


	////calc this current frame offset
	for (uint64_t index = i64LastChunkFrameCount + 1; index < ui64frameindex; index++)
	{
		ui64offset += file->atrack[ui32channelindex].stsz.samplesizetable[index];
	}

	return 0;
}
