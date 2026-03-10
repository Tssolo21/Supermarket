import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import { Login } from './components/Login';
import { Layout } from './components/Layout';
import { Dashboard } from './components/Dashboard';
import { ProductsList } from './components/ProductsList';
import { POS } from './components/POS';

function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/login" element={<Login />} />

        <Route element={<Layout />}>
          <Route path="/" element={<Dashboard />} />
          <Route path="/products" element={<ProductsList />} />
          <Route path="/pos" element={<POS />} />
          <Route path="/categories" element={<div className="animate-fade-in"><h1 className="text-3xl font-bold">Categories</h1></div>} />
          <Route path="/suppliers" element={<div className="animate-fade-in"><h1 className="text-3xl font-bold">Suppliers</h1></div>} />
        </Route>
      </Routes>
    </BrowserRouter>
  );
}

export default App;
