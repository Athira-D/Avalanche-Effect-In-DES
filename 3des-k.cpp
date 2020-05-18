

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
    int x=(int)a;

}
int find_hamming_distance(byte a,byte b)
{   int count=0;
   //cout<<"lll";
  byte t= a^b;
  //cout<<";;;;"<<endl;
  for(int i=0;i<8;i++)
  {
    count=count+(t&1);
    t=t>>1;
    

  }
  return count;

}
int find_avalanche_effect(vector <byte> input,vector <byte> cipher)
{
  //cout<<input.size()<<endl;
  int count=0;
  for(int i=0;i<input.size();i++)
  {
    //cout<<"heerreee"<<endl;
    count=count+find_hamming_distance(input[i],cipher[i]);
    //cout<<i<<" "<<count<<endl;
  }
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

void flip_key(byte * inp,int n)
{
           cout<<n<<"\n";
          //cout<<sizeof(inp);
	string encoded;
          encoded.clear();
	StringSource(inp, 24,true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout<<"key :"<<encoded<<endl;
         // cout<<sizeof(input)<<"\n";
	int byte_no=n/24;
	int bit_pos=n%24;
	byte temp=inp[23-byte_no];
	temp=temp^(1<<bit_pos);
	inp[23-byte_no]=temp;	
        //  return inp;
}

int main(int argc, char* argv[])
{
  
  srand(32);
  float sum=0;
  AutoSeededRandomPool prng;
  SecByteBlock key(DES_EDE3::DEFAULT_KEYLENGTH);
  //cout<<"key size="<<key.size()<<endl;
  prng.GenerateBlock(key, key.size());

  byte iv[DES_EDE3::BLOCKSIZE];
  prng.GenerateBlock(iv, sizeof(iv));
  vector<byte> plain;
  HexEncoder encoder(new FileSink(cout));
  vector <byte> prev_cipher;

  string str("Attacks");
  std::copy(str.begin(), str.end(), std::back_inserter(plain));

  for(int i=0;i<1000;i++)
  {
      cout<<i<<"\n";
    vector<byte> cipher,recover;
    cout << "Plain text: ";
  encoder.Put(plain.data(), plain.size());
  encoder.MessageEnd();
  cout << endl;

  /////////////////////////////////////////////////////////////

  ECB_Mode<DES_EDE3>::Encryption enc;
  enc.SetKey(key,key.size());
  
  // Make room for padding
  cipher.resize(plain.size()+DES_EDE3::BLOCKSIZE);
  ArraySink cs(&cipher[0],cipher.size());

  ArraySource(plain.data(), plain.size(), true,
    new StreamTransformationFilter(enc, new Redirector(cs)));

  // Set cipher text length now that its known
  cipher.resize(cs.TotalPutLength());

  cout << "Cipher text: ";
  encoder.Put(cipher.data(),cipher.size());
  encoder.MessageEnd();
  cout << endl;
  //cout<<cipher.size()<<endl;
   //cout<<plain.size()<<endl;

  /////////////////////////////////////////////////////////////

  ECB_Mode<DES_EDE3>::Decryption dec;
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
  
  //flip_key(key,rand()%191);
  int n=rand()%191;
 ////////////////////////////////////////////////////////////
  	string encoded;
          encoded.clear();
	StringSource(key, key.size(),true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout<<"key :"<<encoded<<endl;
         // cout<<sizeof(input)<<"\n";
         cout<<n<<"\n";
	int byte_no=n/8;
	int bit_pos=n%8;
	byte temp=key[23-byte_no];
	temp=temp^(1<<bit_pos);
	key[23-byte_no]=temp;
//////////////////////////////////////////////////////////////////

  cout<<"\n";
  if(i==0)
  {
    prev_cipher.resize(cipher.size());
    prev_cipher=cipher;
    continue;
  }
  else
  {
    int c;
   
    c=find_avalanche_effect(cipher,prev_cipher);
    //cout<<"Avalanche effect is: "<<float(c)*100/64<<endl;
    sum+=float(c)*100/64;
  }
  prev_cipher.resize(cipher.size());
  prev_cipher=cipher;
  }
  cout<<"Average="<<sum/1000;
  return 0;
}
