// g++ -g3 -ggdb -O0 -DDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread
// g++ -g -O2 -DNDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;
#include<bits/stdc++.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <string>
using std::string;
#include <cstdlib>
using std::exit;
#include<bitset>
#include "cryptlib.h"
using CryptoPP::Exception;
#include "hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;
#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;
#include "des.h"
using CryptoPP::DES;
#include "modes.h"
using CryptoPP::CBC_Mode;
#include "secblock.h"
using CryptoPP::SecByteBlock;

string hextobin(string hex)
{
      string  bin;
      for(int i=0; hex[i]!='\0'; i++)
     {
       	 switch(hex[i])
          {
            case '0':
                bin+= "0000";
                break;
            case '1':
                bin+= "0001";
                break;
            case '2':
                 bin+= "0010";
                break;
            case '3':
                 bin+="0011";
                break;
            case '4':
                bin+= "0100";
                break;
            case '5':
                 bin+= "0101";
                break;
            case '6':
                 bin+= "0110";
                break;
            case '7':
                 bin+="0111";
                break;
            case '8':
                 bin+= "1000";
                break;
            case '9':
                bin+= "1001";
                break;
            case 'a':
            case 'A':
                 bin+= "1010";
                break;
            case 'b':
            case 'B':
                 bin+="1011";
                break;
            case 'c':
            case 'C':
                 bin+="1100";
                break;
            case 'd':
            case 'D':
                 bin+= "1101";
                break;
            case 'e':
            case 'E':
                 bin+="1110";
                break;
            case 'f':
            case 'F':
                 bin+= "1111";
                break;
          }
       }
	return bin;
}

int find_hamming_distance(byte a,byte b)
{   
	int count=0;
	byte t= a^b;
	for(int i=0;i<8;i++)
	{
		count=count+(t&(1<<i));
	}
	return count;
}

int find_avalanche_effect(string c1,string c2)
{
	
	int l1=c1.length();
	int l2=c2.length();
	int count=0;
	if(l1>l2)
	{
              count=l1-l2;
	    for(int i=0;i<l2;i++)
	    {
		if(c1[i]!=c2[i])
		 	count++;
	    }
	}
	else
	{
              count=l2-l1;
	    for(int i=0;i<l1;i++)
	    {
		if(c1[i]!=c2[i])
		 	count++;
	    }
	}
	cout<<count<<"\n";
	float c=count;
	cout<<c/l1*100<<"\n";
	cout<<c/l2*100<<"\n";
	return count;
}

void flip_bit(byte *input,int n)
{
	int byte_no=n/8;
	int bit_pos=n%8;
	byte temp=input[7-byte_no];
	temp=temp^(1<<bit_pos);
	input[7-byte_no]=temp;
	//cout<<input.to_string()<<endl;
}

int main(int argc, char* argv[])
{
	srand (32);  //This program will create different sequence of random numbers on every program run because of srand() fn.
	for(int i=0;i<5;i++)
	{
		int random_bit= rand()%64;
		cout<<"Random Bit "<<random_bit<<endl;
	}
	cout<<"\n";
	AutoSeededRandomPool prng;
	SecByteBlock key(DES::DEFAULT_KEYLENGTH);
	cout<<"key size="<<key.size()<<endl;
	prng.GenerateBlock(key, key.size());

	byte iv[DES::BLOCKSIZE];
	prng.GenerateBlock(iv, sizeof(iv));

	string plain = "CBC Mode Test";
	string cipher1,cipher2, encoded, recovered,p;

	// Pretty print key
	encoded.clear();
	StringSource(key, key.size(), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "key: " << encoded << endl;

	
	// Pretty print iv
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "iv: " << encoded << endl;

    // cout << "key length: " << DES::DEFAULT_KEYLENGTH << endl;
    // cout << "block size: " << DES::BLOCKSIZE << endl;
	byte input[8];
	prng.GenerateBlock(input,sizeof(input));
	encoded.clear();
	StringSource(input,sizeof(input), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "input1: " << encoded << endl;
          plain=encoded;

        
	try
	{
		cout << "plain text: " << plain << endl;

		CBC_Mode< DES >::Encryption e;
		e.SetKeyWithIV(key, key.size(), iv);

		// The StreamTransformationFilter adds padding
		//  as required. ECB and CBC Mode must be padded
		//  to the block size of the cipher.
		StringSource(plain, true, 
			new StreamTransformationFilter(e,
				new StringSink(cipher1)
			) // StreamTransformationFilter      
		); // StringSource
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	

	// Pretty print
	encoded.clear();
	StringSource(cipher1, true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "cipher text1: " << encoded << endl;
	
	string b1;
	b1=hextobin(encoded);
	cout<<b1<<"\n";

	try
	{
		CBC_Mode< DES >::Decryption d;
		d.SetKeyWithIV(key, key.size(), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(cipher1, true, 
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter
		); // StringSource

		cout << "recovered text: " << recovered << endl;
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	cout<<"----------------------------------------\n";
	flip_bit(input,5);
	
	encoded.clear();
	StringSource(input,sizeof(input), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "input2: " << encoded << endl;
          plain=encoded;

	try
	{
		cout << "plain text: " << plain << endl;

		CBC_Mode< DES >::Encryption e;
		e.SetKeyWithIV(key, key.size(), iv);

		// The StreamTransformationFilter adds padding
		//  as required. ECB and CBC Mode must be padded
		//  to the block size of the cipher.
		StringSource(plain, true, 
			new StreamTransformationFilter(e,
				new StringSink(cipher2)
			) // StreamTransformationFilter      
		); // StringSource
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	encoded.clear();
	StringSource(cipher2, true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	recovered.clear();
	cout << "cipher text2: " <<encoded << endl;
	try
	{
		CBC_Mode< DES >::Decryption d;
		d.SetKeyWithIV(key, key.size(), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(cipher2, true, 
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter
		); // StringSource

		cout << "recovered text: " << recovered << endl;
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	cout<<"size 1"<<cipher1.size()<<"\n";
	cout<<"size 2"<<cipher2.size()<<"\n";
	string b2;
	b2=hextobin(encoded);
	cout<<b2<<"\n";		
	find_avalanche_effect(b1,b2);
	return 0;
}

