import React from 'react';

import Header from './Header';
import Dashboard from './Dashboard';

import './App.scss';

import 'typeface-roboto';
import 'typeface-roboto-mono';

const App = () => {
  return (
    <>
      <Header />

      <main>
        <Dashboard />
      </main>
    </>
  );
};

export default App;
