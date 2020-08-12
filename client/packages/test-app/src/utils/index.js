import TransportWebUSB from '@ledgerhq/hw-transport-webusb';
import { listen } from '@ledgerhq/logs';
import CRC32 from 'crc-32';
import { ec } from 'elliptic';
import { keccak256 } from 'js-sha3';

import { HWSDK } from '@coti/hw-sdk';

const DEFAULT_TIMEOUT = 60000;

const connect = async () => {
  const transport = await TransportWebUSB.create();
  transport.setExchangeTimeout(DEFAULT_TIMEOUT);

  return new HWSDK(transport);
};

const toBytesInt32 = num => {
  const arr = new ArrayBuffer(4);
  const view = new DataView(arr);
  view.setInt32(0, num, false);
  return arr;
};

export const byteArrayToHexString = uint8arr => {
  if (!uint8arr) {
    return '';
  }

  let hexStr = '';
  for (let i = 0; i < uint8arr.length; i++) {
    var hex = (uint8arr[i] & 0xff).toString(16);
    hex = hex.length === 1 ? '0' + hex : hex;
    hexStr += hex;
  }

  return hexStr;
};

const paddingPublicKey = (publicKeyX, publicKeyY) => {
  const paddingLetter = '0';
  let publicX = publicKeyX;
  let publicY = publicKeyY;

  if (publicKeyX.length < 64) {
    for (let i = publicKeyX.length; i < 64; i++) {
      publicX = paddingLetter + publicX;
    }
  }

  if (publicKeyY.length < 64) {
    for (let i = publicKeyY.length; i < 64; i++) {
      publicY = paddingLetter + publicY;
    }
  }

  return publicX + publicY;
};

const toAddress = publicKey => {
  const secp256k1 = new ec('secp256k1');

  const key = secp256k1.keyFromPublic(publicKey, 'hex');
  const publicXKeyHex = key.getPublic().x.fromRed().toString(16, 2);
  const publicYKeyHex = key.getPublic().y.fromRed().toString(16, 2);

  const checkSum = CRC32.buf(Buffer.from(publicXKeyHex + publicYKeyHex, 'hex'));
  const checkSum4Bytes = Array.from(new Uint8Array(toBytesInt32(checkSum)));
  const checkSumHex = byteArrayToHexString(checkSum4Bytes);
  const paddedAddress = paddingPublicKey(publicXKeyHex, publicYKeyHex);

  return `${paddedAddress}${checkSumHex}`;
};

const toUserHash = publicKey => {
  const secp256k1 = new ec('secp256k1');

  const key = secp256k1.keyFromPublic(publicKey, 'hex');
  const publicXKeyHex = key.getPublic().x.fromRed().toString(16, 2);
  const publicYKeyHex = key.getPublic().y.fromRed().toString(16, 2);

  return paddingPublicKey(publicXKeyHex, publicYKeyHex);
};

const getPublicKey = async (index, interactive) => {
  const hw = await connect();

  const res = await hw.getPublicKey(index, interactive);
  return res.publicKey;
};

const getUserPublicKey = async interactive => {
  const hw = await connect();

  const res = await hw.getUserPublicKey(false);
  return res.publicKey;
};

export const getAddressInfo = async (index, interactive) => {
  const publicKey = await getPublicKey(index, interactive);
  const userPublicKey = await getUserPublicKey(false);

  return { publicKey, userHash: toUserHash(userPublicKey), address: toAddress(publicKey) };
};

export const signMessage = async (index, message) => {
  const hw = await connect();
  const messageHex = Buffer.from(message).toString('hex');
  const res = await hw.signMessage(index, messageHex);

  return { v: res.v, r: res.r, s: res.s };
};

export const signUserMessage = async message => {
  const hw = await connect();
  const messageHex = Buffer.from(message).toString('hex');
  const res = await hw.signUserMessage(messageHex);

  return { v: res.v, r: res.r, s: res.s };
};

const verifyPublicKeySignature = (publicKey, message, r, s) => {
  const secp256k1 = new ec('secp256k1');
  const key = secp256k1.keyFromPublic(publicKey, 'hex');
  const signature = { r, s };
  const messageHash = keccak256.digest(message);

  return key.verify(messageHash, signature);
};

export const verifySignature = async (index, message, r, s) => {
  const publicKey = await getPublicKey(index, false);

  return verifyPublicKeySignature(publicKey, message, r, s);
};

export const verifyUserSignature = async (message, r, s) => {
  const userPublicKey = await getUserPublicKey(false);

  return verifyPublicKeySignature(userPublicKey, message, r, s);
};

listen(log => console.log(`${log.type}: ${log.message}`));
