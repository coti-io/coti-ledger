import React, { useState } from 'react';
import { Form, Row, Col, Button, FormGroup, FormLabel, FormControl, InputGroup } from 'react-bootstrap';

import { keccak256 } from 'js-sha3';
import validator from 'validator';
import { BIP32_PATH } from '@coti/hw-sdk';

import { signMessage, signUserMessage, verifySignature, verifyUserSignature, byteArrayToHexString } from '../utils';

import ErrorModal from './ErrorModal';
import Loader from './Loader';
import CopyToClipboard from './CopyToClipboard';

const STATUSES = {
  OK: 'ok',
  INVALID: 'invalid',
  WARNING: 'warning',
  UNDEFINED: 'undefined'
};

const SignMessage = () => {
  const [loading, setLoading] = useState(false);
  const [index, setIndex] = useState(0);
  const [indexEnabled, setIndexEnabled] = useState(true);
  const [userPublicKey, setUserPublicKey] = useState(false);
  const [messageHex, setMessageHex] = useState('');
  const [messageHexHash, setMessageHexHash] = useState('');
  const [v, setV] = useState('');
  const [r, setR] = useState('');
  const [s, setS] = useState('');
  const [status, setStatus] = useState({
    status: STATUSES.UNDEFINED,
    message: ''
  });
  const [error, setError] = useState({});

  const isHexData = value => {
    if (value.length % 2 !== 0) {
      return false;
    }

    let hex = false;
    let tmp = value;
    if (tmp) {
      if (tmp.startsWith('0x')) {
        tmp = tmp.slice(2) || '0';
      }
      hex = validator.isHexadecimal(tmp);
    }

    return hex;
  };

  const onChangeIndex = ({ target }) => {
    const element = target;
    const { value } = element;

    setIndex(value);
  };

  const onChangeUserPublicKey = ({ target }) => {
    const element = target;
    const { checked } = element;

    setIndexEnabled(!checked);
    setUserPublicKey(checked);
  };

  const onChangeMessageHex = ({ target }) => {
    const element = target;
    const { value } = element;

    setMessageHex(value);

    if (isHexData(value)) {
      setMessageHexHash(byteArrayToHexString(keccak256.digest(Buffer.from(value, 'hex'))));
    } else {
      setMessageHexHash('');
    }
  };

  const onSubmit = async event => {
    event.preventDefault();

    if (!isHexData(messageHex)) {
      setError({
        show: true,
        title: 'Invalid input',
        message: 'Not a valid hexadecimal data'
      });

      return;
    }

    setLoading(true);

    try {
      setStatus({
        status: STATUSES.UNDEFINED,
        message: ''
      });

      const { v, r, s } = userPublicKey ? await signUserMessage(messageHex) : await signMessage(index, messageHex);

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

  const onVerifySubmit = async event => {
    event.preventDefault();

    setLoading(true);

    try {
      const res = userPublicKey
        ? await verifyUserSignature(messageHex, r, s)
        : await verifySignature(index, messageHex, r, s);
      if (res) {
        setStatus({
          status: STATUSES.OK,
          message: 'Message signature verified'
        });
      } else {
        setStatus({ status: STATUSES.INVALID, message: 'Invalid signature' });
      }
    } catch (err) {
      setStatus({ status: STATUSES.INVALID, message: err.message });
    }

    setLoading(false);
  };

  const statusClassName = () => {
    switch (status.status) {
      case STATUSES.OK:
        return 'is-valid';

      case STATUSES.INVALID:
        return 'is-invalid';

      case STATUSES.WARNING:
        return 'is-invalid';

      default:
        return;
    }
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
            <Form.Control
              className="form-control"
              type="number"
              value={index}
              readOnly={!indexEnabled}
              onChange={onChangeIndex}
            />
            <small className="form-text">The index to derive the {BIP32_PATH}/index BIP32 path</small>
          </Col>
        </FormGroup>

        <FormGroup as={Row}>
          <Col md={{ span: 9, offset: 2 }}>
            <InputGroup className="mb-3">
              <Form.Check
                type="checkbox"
                value={userPublicKey}
                label="Sign with user hash public key"
                onChange={onChangeUserPublicKey}
              />
            </InputGroup>
          </Col>
        </FormGroup>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Message Hex</FormLabel>
          </Col>
          <Col md={9}>
            <Form.Control
              className="form-control"
              as="textarea"
              rows="3"
              value={messageHex}
              onChange={onChangeMessageHex}
            />
            <small className="form-text">The non-hashed message hex to sign</small>
          </Col>
        </FormGroup>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Message SHA3</FormLabel>
          </Col>
          <Col md={9}>
            <Form.Control className="form-control bytes" type="text" value={messageHexHash} readOnly={true} />
          </Col>
        </FormGroup>

        <Button type="submit">Sign Message</Button>
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

      <Form className="component-result" onSubmit={onVerifySubmit}>
        <h5>External Verification</h5>

        <FormGroup as={Row}>
          <Col md={2}>
            <FormLabel>Result</FormLabel>
          </Col>
          <Col md={9}>
            <InputGroup className="mb-3">
              <FormControl type="text" className={statusClassName()} value={status.message} readOnly={true} />
            </InputGroup>
          </Col>
        </FormGroup>

        <Button type="submit">Verify Signature</Button>
      </Form>

      <ErrorModal show={show} title={title} message={message} onHide={() => setError({})} />
    </>
  );
};

export default SignMessage;
