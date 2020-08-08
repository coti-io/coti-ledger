import React from 'react';

import { Modal } from 'react-bootstrap';

import './ErrorModal.scss';

const ErrorModal = ({ title, message, ...props }) => {
  return (
    <Modal {...props} className="modal-error" size="md" aria-labelledby="contained-modal-title-vcenter" centered>
      <Modal.Header closeButton>
        <Modal.Title className="centered" id="contained-modal-title-vcenter">
          {title}
        </Modal.Title>
      </Modal.Header>
      <Modal.Body>
        <p className="centered">{message}</p>
      </Modal.Body>
    </Modal>
  );
};

export default ErrorModal;
