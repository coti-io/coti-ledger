import React from 'react';

import { Row, Col, Image } from 'react-bootstrap';

import avatar from '../images/coti.png';

import './Header.scss';

const Header = () => {


  return (
    <header>
      <div className="container-fluid">
        <div className="header-body">
          <Row>
            <Col md="auto">
              <div className="avatar">
                <Image src={avatar} className="avatar-img" />
              </div>
            </Col>
            <Col>
              <h1 className="header-title">COTI HW Test App</h1>
              <h6 className="header-version">{process.env.REACT_APP_VERSION}</h6>
            </Col>
          </Row>
        </div>
      </div>
    </header>
  );
};

export default Header;
