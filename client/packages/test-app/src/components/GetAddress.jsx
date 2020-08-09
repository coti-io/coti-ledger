import React, { useState } from 'react';
import { Form, Row, Col, Button, FormGroup, FormLabel, FormControl, InputGroup } from 'react-bootstrap';

import { BIP32_PATH } from '@coti/hw-sdk';
import { getAddressInfo } from '../utils';

import ErrorModal from './ErrorModal';
import Loader from './Loader';
import CopyToClipboard from './CopyToClipboard';

const GetAddress = () => {
  const [loading, setLoading] = useState(false);
  const [index, setIndex] = useState(0);
  const [publicKey, setPublicKey] = useState('');
  const [address, setAddress] = useState('');
  const [error, setError] = useState({});

  const onChangeIndex = ({ target }) => {
    const element = target;
    const { value } = element;

    setIndex(value);
  };

  const onSubmit = async event => {
    event.preventDefault();

    setLoading(true);

    const path = `${BIP32_PATH}/${index}`;

    try {
      const { publicKey, address } = await getAddressInfo(path);

      setPublicKey(publicKey);
      setAddress(address);
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

export default GetAddress;
