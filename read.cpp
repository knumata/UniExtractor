#include <iostream>

using namespace std;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long long int llu;

uint getInt(char *buf)
{
	uint ret = 0;
	for (int i=0; i<4; i++)
	{
		// printf("%X\n", (uchar)buf[i]);	
		ret = (ret << 8 ) + (((uint)buf[i]) & 0xff);
	}
	return ret;
}

uint getShort(char *buf)
{
	uint ret = 0;
	for (int i=0; i<2; i++)
	{
		// printf("%X\n", (uchar)buf[i]);	
		ret = (ret << 8) + (((uint)buf[i]) & 0xff);
	}
	return ret;
}

uint getBInt(char *buf)
{
	uint ret = 0;
	for (int i=3; i>=0; i--)
	{
		// printf("%X\n", (uchar)buf[i]);	
		ret = (ret << 8 ) + (((uint)buf[i]) & 0xff);
	}
	return ret;
}

uint getBShort(char *buf)
{
	uint ret = 0;
	for (int i=1; i>=0; i--)
	{
		// printf("%X\n", (uchar)buf[i]);	
		ret = (ret << 8 ) + (((uint)buf[i]) & 0xff);
	}
	return ret;
}

llu getBLong(char *buf)
{
	llu ret = 0;
	for (int i=7; i>=0; i--)
	{
		// printf("%X\n", (uchar)buf[i]);	
		ret = (ret << 8 ) + (((uint)buf[i]) & 0xff);
	}
	return ret;
}

void experimentalPrint(char *buf, int sz)
{
	for (int i=0; i<sz; i++)
	{
		printf("%02X", (uchar)(buf[i]));
	}
	cout << endl;
}

void printSignature(char *buf)
{
	experimentalPrint(buf, 16);
}

int main(void)
{

	while (cin)
	{
		char buf[19];
		cin.read(buf, 18);	

		/*
		int metadataSize = ((int *)buf)[0];
		int fileSize = ((int *)buf)[1];
		int version = ((int *) buf)[2];
		int offset = ((int *) buf)[3];
		short int magic = *(short int *)(buf + 16);
		*/

		int metadataSize = getInt(buf);
		int fileSize = getInt(buf+4);
		int version = getInt(buf+8);
		int offset = getInt(buf+12);
		short int magic = getShort(buf+16);

		cout <<  "meta size: " << metadataSize << endl;
		cout <<  "filesize: " << fileSize << endl;
		cout <<  "version: " << version << endl;
		cout <<  "offset: " << offset << endl;
		cout <<  "magic: " << magic << endl;

		char meta[metadataSize+1];
		cin.read(meta, metadataSize);

		// skip the first 2 bytes (magic)
		experimentalPrint(meta, 2);

		char *unityVersion = meta+2;
		printf("Unity Version: %s\n", unityVersion);

		printf("TypeTreeMagic: %X\n", *(uchar*)(meta+10));

		// skip 4 bytes
		experimentalPrint(meta+11, 4);

		int numBaseClasses = getBInt(meta+15);
		cout << "numBaseClasses: " << numBaseClasses << endl;

		char *bcBuf = meta + 19;
		for (int bc =0; bc < numBaseClasses; bc++)
		{
			cout << "ClassID[" << bc << "]:" << endl;
			// classID(4) + normal(16) + extra(16) + NULL
			//char bcBuf[37];
			//cin.read(bcBuf, 4+16);
			//bcBuf[20] = 0;
			int classID = (int)getBInt(bcBuf);
			cout << "    ClassID: " << classID << endl;
			cout << "    Signature : ";
			bcBuf += 4;
			printSignature(bcBuf);
			if (classID < 0)
			{
				bcBuf += 16;
				cout << "    Signature2: ";
				printSignature(bcBuf);			
			}

			bcBuf += 16;
		}

		int numObjects = getBInt(bcBuf);
		cout << "numObjects: " << numObjects << endl;
		char *oiBuf = bcBuf+4;	
		//printf("## addr: %0llX\n", (long long int)oiBuf);

		//int al = ((long long unsigned int)oiBuf) % 4;
		int al = 1;
		//printf("## alignment: %d\n", al);
		oiBuf += 4 - al;

		// printf("## addr: %0llX\n", (long long int)oiBuf);

		for (int oi = 0; oi < numObjects; oi++)
		{
			llu objectID = getBLong(oiBuf);
			cout << "ObjectID[" << oi << "]: " << objectID << endl;
			oiBuf += 8;

			int byteStart = (int)getBInt(oiBuf);
			cout << "    byteStart: " << byteStart << endl;
			oiBuf += 4;

			int byteSize = (int)getBInt(oiBuf);
			cout << "    byteSize: " << byteSize << endl;
			oiBuf += 4;
			
			int typeID = (int)getBInt(oiBuf);
			cout << "    TypeID: " << typeID << endl;
			oiBuf += 4;

			int oClassID = (int)getBShort(oiBuf);
			cout << "    ClassID: " << oClassID << endl;
			oiBuf += 2;
			
			int oMagic = (int)getBShort(oiBuf);
			cout << "    Magic: " << oMagic << endl;
			oiBuf += 2;

			if (oi != numObjects-1)
			{
				// skip 4 bytes 00 00 00 00
				oiBuf += 4;
				continue;
			}

			// experimentalPrint(oiBuf, 2);
			int objMagic = getShort(oiBuf);

			if (objMagic != 0)
			{
				cout << "extra Obj: " << objMagic << endl;
				int ck = 12;
				oiBuf += 2;


				for(int k=0; k<objMagic; k++)
				{
					cout << "    extra[" << k << "]: ";
					experimentalPrint(oiBuf, ck);
					oiBuf += ck;
				}

				experimentalPrint(oiBuf, 5);
				oiBuf += 5;

				// experimentalPrint(oiBuf, 64);
			}
			else
			{
				oiBuf += 4;
			}
		}

		char *extBuf = oiBuf;
		int numExternals = getShort(extBuf);
		cout  << "numExternals: " << numExternals << endl;

		extBuf += 2;

		cout << "(padding) ";
		experimentalPrint(extBuf, 2);

		extBuf += 2;

		for (int etr = 0; etr < numExternals; etr++)
		{
			cout << "    External[" << etr << "]: ";
			experimentalPrint(extBuf, 2);
			extBuf += 2;

			cout << "    GUID: ";
			experimentalPrint(extBuf, 16);
			extBuf += 16;

			cout << "    magic: ";
			experimentalPrint(extBuf, 4);
			extBuf += 4;

			/*
			int magicPad = getInt(extBuf);

			if (magicPad != 0)
			{
				extBuf += 2;
			} 
			else
			{
				extBuf += 4;
			}
			*/
	
			int pathLen = strnlen(extBuf, 1024);
			cout << "    path: " << extBuf << endl;
			extBuf += pathLen;
		}


		break;

		/*
		string str;
		getline(cin, str);
		cout << "## " << str << endl;
		*/
	}


	return 0;
}
