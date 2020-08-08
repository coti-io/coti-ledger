import React, { useState } from 'react';
import { Form, Row, Col, Button, FormGroup, FormLabel, FormControl, InputGroup } from 'react-bootstrap';

import TransportWebUSB from '@ledgerhq/hw-transport-webusb';
import { listen } from '@ledgerhq/logs';
import CRC32 from 'crc-32';
import { ec } from 'elliptic';
import { HWSDK, BIP32_PATH } from '@coti/hw-sdk';

import ErrorModal from './ErrorModal';
import Loader from './Loader';
import CopyToClipboard from './CopyToClipboard';

const DEFAULT_TIMEOUT = 60000;

const GetAddress = () => {
  const [loading, setLoading] = useState(false);
  const [index, setIndex] = useState(0);
  const [publicKey, setPublicKey] = useState('');
  const [address, setAddress] = useState('');
  const [error, setError] = useState({});

  const connect = async () => {
    const transport = await TransportWebUSB.create();
    transport.setExchangeTimeout(DEFAULT_TIMEOUT);

    return new HWSDK(transport);
  };

  const onChangeIndex = ({ target }) => {
    const element = target;
    const { value } = element;

    setIndex(value);
  };

  const onSubmit = async event => {
    event.preventDefault();

    setLoading(true);

    const path = `${BIP32_PATH}/${index}`;
    let results;
    try {
      const hw = await connect();
      results = await hw.getAddress(path);

      const { publicKey } = results;
      setPublicKey(publicKey);
      setAddress(toAddress(publicKey));
    } catch (err) {
      console.error(`${err.name}: ${err.message}`);

      setError({
        show: true,
        title: err.name,
        message: err.message
      });
    }

    setLoading(false);
  };

  const toBytesInt32 = num => {
    const arr = new ArrayBuffer(4);
    const view = new DataView(arr);
    view.setInt32(0, num, false);
    return arr;
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

  const byteArrayToHexString = uint8arr => {
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

  const { show, title, message } = error;

  return (
    <>
      <Loader loading={loading} />

      <Form className="component-form" onSubmit={onSubmit}>
        <h5>Input</h5>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Address Index</FormLabel>
          </Col>
          <Col md={9}>
            <Form.Control className="form-control" type="number" value={index} onChange={onChangeIndex} />
            <small className="form-text">The index to derive the ${BIP32_PATH}/index BIP32 path</small>
          </Col>
        </FormGroup>

        <Button type="submit">Submit</Button>
      </Form>

      <Form className="component-result">
        <h5>Output</h5>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Public Key</FormLabel>
          </Col>
          <Col md={9}>
            <InputGroup className="mb-3">
              <FormControl className="key" as="textarea" rows="3" value={publicKey} readOnly={true} />
              <InputGroup.Append>
                <CopyToClipboard text={publicKey} />
              </InputGroup.Append>
            </InputGroup>
          </Col>
        </FormGroup>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Address</FormLabel>
          </Col>
          <Col md={9}>
            <InputGroup className="mb-3">
              <FormControl className="address" as="textarea" rows="3" value={address} readOnly={true} />
              <InputGroup.Append>
                <CopyToClipboard text={address} />
              </InputGroup.Append>
            </InputGroup>
          </Col>
        </FormGroup>
      </Form>

      <ErrorModal show={show} title={title} message={message} onHide={() => setError({})} />
    </>
  );
};

listen(log => console.log(`${log.type}: ${log.message}`));

export default GetAddress;
