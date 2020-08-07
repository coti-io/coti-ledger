import React, { useState } from 'react';
import { Form, Row, Col, Button, FormGroup, FormLabel, FormControl, InputGroup } from 'react-bootstrap';

import TransportWebUSB from '@ledgerhq/hw-transport-webusb';
import { listen } from '@ledgerhq/logs';
import { HWSDK, BIP32_PATH } from '@coti/hw-sdk';

import Loader from './Loader';
import CopyToClipboard from './CopyToClipboard';

const DEFAULT_TIMEOUT = 60000;

const GetAddress = () => {
  const [loading, setLoading] = useState(false);
  const [index, setIndex] = useState(0);
  const [publicKey, setPublicKey] = useState('');
  const [address, setAddress] = useState('');

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
    debugger;
    let results;
    try {
      const hw = await connect();
      results = await hw.getAddress(path);
      debugger;
    } catch (err) {
      console.error(`${err.name}: ${err.message}`);
      debugger;

      // let message = err.message;
      // setError({
      //   show: true,
      //   title: 'Sending Failed',
      //   message
      // });
    }

    setLoading(false);

    setPublicKey(results.publicKey);
    setAddress('Kuku ' + Date.now());
  };

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
              <FormControl className="key" type="text" value={publicKey} readOnly={true} />
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
              <FormControl className="address" type="text" value={address} readOnly={true} />
              <InputGroup.Append>
                <CopyToClipboard text={address} />
              </InputGroup.Append>
            </InputGroup>
          </Col>
        </FormGroup>
      </Form>
    </>
  );
};

listen(log => console.log(`${log.type}: ${log.message}`));

export default GetAddress;
