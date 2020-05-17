#include <iostream>
#include <string>
#include "osrng.h"

using namespace std;


#include "cryptlib.h"
#include "filters.h"
#include "files.h"
#include "modes.h"
#include "hex.h"
#include "des.h"
#include "secblock.h"
using namespace CryptoPP;

string find_binstring(byte a)
{

}
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
{   int count=0;
   cout<<"lll";
  byte t= a^b;
  cout<<";;;;"<<endl;
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

vector<byte> flip_bit(vector <byte> input,int n)
{
  int byte_no=n/8;
  int bit_pos=n%8;
  byte temp=input[7-byte_no];
  temp=temp^(1<<bit_pos);
  input[7-byte_no]=temp;
  return input;

}

int main(int argc, char* argv[])
{
  AutoSeededRandomPool prng;
  SecByteBlock key(DES::DEFAULT_KEYLENGTH);
  cout<<"key size="<<key.size()<<endl;
  prng.GenerateBlock(key, key.size());

  byte iv[DES::BLOCKSIZE];
  prng.GenerateBlock(iv, sizeof(iv));
  vector<byte> plain;
  HexEncoder encoder(new FileSink(cout));
  vector <byte> prev_cipher;

  string str("Attacks!");
  std::copy(str.begin(), str.end(), std::back_inserter(plain));

  for(int i=0;i<2;i++)
  {
    vector<byte> cipher,recover;
    cout << "Plain text: ";
  encoder.Put(plain.data(), plain.size());
  encoder.MessageEnd();
  cout << endl;

  /////////////////////////////////////////////////////////////

  ECB_Mode<DES>::Encryption enc;
  enc.SetKey(key,key.size());

  // Make room for padding
  cipher.resize(plain.size()+DES::BLOCKSIZE);
  ArraySink cs(&cipher[0],cipher.size());

  ArraySource(plain.data(), plain.size(), true,
    new StreamTransformationFilter(enc, new Redirector(cs)));

  // Set cipher text length now that its known
  cipher.resize(cs.TotalPutLength());

  cout << "Cipher text: ";
  encoder.Put(cipher.data(),cipher.size());
  encoder.MessageEnd();
  cout << endl;
  cout<<cipher.size()<<endl;
   cout<<plain.size()<<endl;

  /////////////////////////////////////////////////////////////

  ECB_Mode<DES>::Decryption dec;
  dec.SetKey(key,key.size());

  // Recovered text will be less than cipher text
  recover.resize(cipher.size());
  ArraySink rs(&recover[0], recover.size());

  ArraySource(cipher.data(), cipher.size(), true,
    new StreamTransformationFilter(dec, new Redirector(rs)));

  // Set recovered text length now that its known
  recover.resize(rs.TotalPutLength());

  cout << "Recovered text: ";
  encoder.Put(recover.data(), recover.size());
  encoder.MessageEnd();
  cout << endl;

  vector<byte> new_plain=flip_bit(plain,1);
  plain=new_plain;
  if(i==0)
   {
    prev_cipher=cipher;
  }
  else
  {
   int d;
   string encoded,b1,b2;
   byte b[8];
   b[0]=cipher[0];
   b[1]=cipher[1];
   b[2]=cipher[2];
   b[3]=cipher[3];
   b[4]=cipher[4];
   b[5]=cipher[5];
   b[6]=cipher[6];
   b[7]=cipher[7];
  
   encoded.clear();
   StringSource(b, sizeof(b),true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
    cout << "b1: " << encoded << endl;
   b1=hextobin(encoded);
   byte c[8];
   c[0]=prev_cipher[0];
   c[1]=prev_cipher[1];
   c[2]=prev_cipher[2];
   c[3]=prev_cipher[3];
   c[4]=prev_cipher[4];
   c[5]=prev_cipher[5];
   c[6]=prev_cipher[6];
   c[7]=prev_cipher[7];

   encoded.clear();
   StringSource(c, sizeof(c),true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
    cout << "b2: " << encoded << endl;
   b2=hextobin(encoded);
  
   cout<<b1<<"\n";
   cout<<b2<<"\n";
   d=find_avalanche_effect(b1,b2);
   // cout<<"Avalanche effect is: "<<endl;
  }
  prev_cipher.resize(cipher.size());
  prev_cipher=cipher;
  }

  return 0;
}
