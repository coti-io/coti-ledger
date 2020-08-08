import React, { useState } from 'react';
import { Form, Row, Col, Button, FormGroup, FormLabel, FormControl, InputGroup } from 'react-bootstrap';

import TransportWebUSB from '@ledgerhq/hw-transport-webusb';
import { listen } from '@ledgerhq/logs';
import { HWSDK, BIP32_PATH } from '@coti/hw-sdk';

import { keccak256 } from 'js-sha3';

import ErrorModal from './ErrorModal';
import Loader from './Loader';
import CopyToClipboard from './CopyToClipboard';

import { byteArrayToHexString } from '../utils';

const DEFAULT_TIMEOUT = 60000;

const SignMessage = () => {
  const [loading, setLoading] = useState(false);
  const [index, setIndex] = useState(0);
  const [message, setMessage] = useState('');
  const [messageHash, setMessageHash] = useState('');
  const [v, setV] = useState('');
  const [r, setR] = useState('');
  const [s, setS] = useState('');
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

  const onChangeMessage = ({ target }) => {
    const element = target;
    const { value } = element;

    setMessage(value);
    setMessageHash(byteArrayToHexString(keccak256.digest(value)));
  };

  const onSubmit = async event => {
    event.preventDefault();

    setLoading(true);

    const path = `${BIP32_PATH}/${index}`;
    let res;
    try {
      const hw = await connect();

      const messageHex = Buffer.from(message).toString('hex');
      res = await hw.signMessage(path, messageHex);

      const { v, r, s } = res;
      setV(v);
      setR(r);
      setS(s);
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

  const { show, title, message: errMsg } = error;

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

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Message</FormLabel>
          </Col>
          <Col md={9}>
            <Form.Control className="form-control" as="textarea" rows="3" value={message} onChange={onChangeMessage} />
            <small className="form-text">The non-hashed message to sign</small>
          </Col>
        </FormGroup>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Message SHA3</FormLabel>
          </Col>
          <Col md={9}>
            <Form.Control className="form-control bytes" type="text" value={messageHash} readOnly={true} />
          </Col>
        </FormGroup>

        <Button type="submit">Submit</Button>
      </Form>

      <Form className="component-result">
        <h5>Output</h5>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>V Component</FormLabel>
          </Col>
          <Col md={9}>
            <InputGroup className="mb-3">
              <FormControl className="signature" type="text" value={v} readOnly={true} />
              <InputGroup.Append>
                <CopyToClipboard text={v} />
              </InputGroup.Append>
            </InputGroup>
          </Col>
        </FormGroup>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>R Component</FormLabel>
          </Col>
          <Col md={9}>
            <InputGroup className="mb-3">
              <FormControl className="signature" type="text" value={r} readOnly={true} />
              <InputGroup.Append>
                <CopyToClipboard text={r} />
              </InputGroup.Append>
            </InputGroup>
          </Col>
        </FormGroup>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>S Component</FormLabel>
          </Col>
          <Col md={9}>
            <InputGroup className="mb-3">
              <FormControl className="signature" type="text" value={s} readOnly={true} />
              <InputGroup.Append>
                <CopyToClipboard text={s} />
              </InputGroup.Append>
            </InputGroup>
          </Col>
        </FormGroup>
      </Form>

      <ErrorModal show={show} title={title} message={errMsg} onHide={() => setError({})} />
    </>
  );
};

listen(log => console.log(`${log.type}: ${log.message}`));

export default SignMessage;
