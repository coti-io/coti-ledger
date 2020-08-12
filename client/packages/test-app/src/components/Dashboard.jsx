import React from 'react';
import { Tab, Row, Col, Nav } from 'react-bootstrap';

import GetAddress from './GetAddress';
import SignMessage from './SignMessage';

import './Dashboard.scss';

const Dashboard = () => {
  return (
    <div className="dashboard">
      <Tab.Container defaultActiveKey="get-address">
        <Row>
          <Col md={2}>
            <Nav variant="pills" className="flex-column">
              <div className="section">
                <Nav.Item>
                  <Nav.Link eventKey="get-address">Get Address</Nav.Link>
                </Nav.Item>

                <Nav.Item>
                  <Nav.Link eventKey="sign-message">Sign Message</Nav.Link>
                </Nav.Item>
              </div>
            </Nav>
          </Col>

          <Col md={9} className="component">
            <Tab.Content>
              <Tab.Pane eventKey="get-address">
                <GetAddress />
              </Tab.Pane>

              <Tab.Pane eventKey="sign-message">
                <SignMessage />
              </Tab.Pane>
            </Tab.Content>
          </Col>
        </Row>
      </Tab.Container>
    </div>
  );
};

export default Dashboard;
