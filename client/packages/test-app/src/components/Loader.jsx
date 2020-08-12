import React from 'react';

import { BounceLoader } from 'react-spinners';

import { Modal } from 'react-bootstrap';

import './Loader.scss';

const Loader = ({ loading }) => {
  return (
    <Modal className="loader" show={loading} size="lg" centered>
      <Modal.Body className="d-flex justify-content-center">
        <BounceLoader loading={loading} size={150} color="#3d6989" />
      </Modal.Body>
    </Modal>
  );
};

export default Loader;
