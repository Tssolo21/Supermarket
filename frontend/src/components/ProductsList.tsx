import React, { useEffect, useState } from 'react';
import { Plus, Search, Loader2, Target, Barcode, DollarSign, Package } from 'lucide-react';
import api from '../lib/api';

interface Product {
    id: number;
    name: string;
    sku: string;
    price: number;
    stock: number;
    category_id: number;
    supplier_id: number;
    description: string;
}

export function ProductsList() {
    const [products, setProducts] = useState<Product[]>([]);
    const [loading, setLoading] = useState(true);
    const [search, setSearch] = useState('');

    useEffect(() => {
        fetchProducts();
    }, []);

    const fetchProducts = async () => {
        try {
            setLoading(true);
            const res = await api.get('/products');
            setProducts(res.data);
        } catch (err) {
            console.error(err);
        } finally {
            setLoading(false);
        }
    };

    const handleSearch = async (e: React.FormEvent) => {
        e.preventDefault();
        if (!search.trim()) {
            return fetchProducts();
        }
        try {
            setLoading(true);
            const res = await api.get(`/products/search?q=${encodeURIComponent(search)}`);
            setProducts(res.data);
        } catch (err) {
            console.error(err);
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className="space-y-6 animate-fade-in">
            <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
                <div>
                    <h1 className="text-3xl font-bold tracking-tight">Products Management</h1>
                    <p className="text-textMuted mt-1">Manage your inventory and stock levels</p>
                </div>
                <button className="btn-primary flex items-center gap-2">
                    <Plus className="w-5 h-5" />
                    Add Product
                </button>
            </div>

            <div className="glass-panel p-4">
                <form onSubmit={handleSearch} className="flex gap-2">
                    <div className="relative flex-1">
                        <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-textMuted/50" />
                        <input
                            type="text"
                            placeholder="Search products by name..."
                            className="input-field pl-10"
                            value={search}
                            onChange={(e) => setSearch(e.target.value)}
                        />
                    </div>
                    <button type="submit" className="btn-secondary">Search</button>
                </form>
            </div>

            <div className="glass-panel overflow-hidden">
                {loading ? (
                    <div className="flex justify-center items-center p-12">
                        <Loader2 className="w-8 h-8 text-primary animate-spin" />
                    </div>
                ) : (
                    <div className="overflow-x-auto">
                        <table className="w-full text-left border-collapse">
                            <thead>
                                <tr className="border-b border-border bg-surface/50">
                                    <th className="p-4 font-semibold text-textMuted text-sm">ID</th>
                                    <th className="p-4 font-semibold text-textMuted text-sm">Product Name</th>
                                    <th className="p-4 font-semibold text-textMuted text-sm">SKU</th>
                                    <th className="p-4 font-semibold text-textMuted text-sm">Price</th>
                                    <th className="p-4 font-semibold text-textMuted text-sm">Stock Level</th>
                                    <th className="p-4 font-semibold text-textMuted text-sm text-right">Actions</th>
                                </tr>
                            </thead>
                            <tbody>
                                {products.length === 0 ? (
                                    <tr>
                                        <td colSpan={6} className="p-8 text-center text-textMuted">
                                            No products found.
                                        </td>
                                    </tr>
                                ) : (
                                    products.map((p) => (
                                        <tr key={p.id} className="border-b border-border/50 hover:bg-surface/30 transition-colors">
                                            <td className="p-4 text-textMuted">#{p.id}</td>
                                            <td className="p-4 font-medium">{p.name}</td>
                                            <td className="p-4">
                                                <span className="inline-flex items-center gap-1.5 px-2.5 py-1 rounded-md bg-surface border border-border text-xs font-mono text-textMuted">
                                                    <Barcode className="w-3 h-3" />
                                                    {p.sku}
                                                </span>
                                            </td>
                                            <td className="p-4 font-medium">${p.price.toFixed(2)}</td>
                                            <td className="p-4">
                                                <span className={`inline-flex items-center gap-1.5 px-2.5 py-1 rounded-full text-xs font-medium ${p.stock > 20 ? 'bg-success/10 text-success border border-success/20' :
                                                        p.stock > 0 ? 'bg-warning/10 text-warning border border-warning/20' :
                                                            'bg-danger/10 text-danger border border-danger/20'
                                                    }`}>
                                                    <Package className="w-3.5 h-3.5" />
                                                    {p.stock} units
                                                </span>
                                            </td>
                                            <td className="p-4 text-right">
                                                <button className="text-primary hover:text-primaryHover text-sm font-medium mr-3 transition-colors">Edit</button>
                                                <button className="text-danger hover:text-danger/80 text-sm font-medium transition-colors">Delete</button>
                                            </td>
                                        </tr>
                                    ))
                                )}
                            </tbody>
                        </table>
                    </div>
                )}
            </div>
        </div>
    );
}
