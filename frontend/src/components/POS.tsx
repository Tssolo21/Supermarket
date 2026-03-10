import React, { useState, useEffect } from 'react';
import { ShoppingCart, Search, Plus, Minus, Trash2, CreditCard, Banknote, Smartphone } from 'lucide-react';
import api from '../lib/api';

interface Product {
    id: number;
    name: string;
    price: number;
    stock: number;
}

interface CartItem extends Product {
    quantity: number;
}

export function POS() {
    const [products, setProducts] = useState<Product[]>([]);
    const [cart, setCart] = useState<CartItem[]>([]);
    const [search, setSearch] = useState('');
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        fetchProducts();
    }, []);

    const fetchProducts = async () => {
        try {
            const res = await api.get('/products');
            setProducts(res.data);
        } catch (err) {
            console.error(err);
        } finally {
            setLoading(false);
        }
    };

    const filteredProducts = products.filter(p => p.name.toLowerCase().includes(search.toLowerCase()));

    const addToCart = (product: Product) => {
        setCart(prev => {
            const existing = prev.find(item => item.id === product.id);
            if (existing) {
                if (existing.quantity >= product.stock) return prev; // Cannot exceed stock
                return prev.map(item => item.id === product.id ? { ...item, quantity: item.quantity + 1 } : item);
            }
            return [...prev, { ...product, quantity: 1 }];
        });
    };

    const updateQuantity = (id: number, delta: number) => {
        setCart(prev => prev.map(item => {
            if (item.id === id) {
                const newQ = item.quantity + delta;
                if (newQ > 0 && newQ <= item.stock) return { ...item, quantity: newQ };
            }
            return item;
        }));
    };

    const removeFromCart = (id: number) => {
        setCart(prev => prev.filter(item => item.id !== id));
    };

    const total = cart.reduce((sum, item) => sum + (item.price * item.quantity), 0);

    const handleCheckout = async (method: string) => {
        if (cart.length === 0) return;

        try {
            const res = await api.post('/checkout', {
                items: cart.map(item => ({ product_id: item.id, quantity: item.quantity })),
                cashier: "Admin",
                payment_method: method
            });

            if (res.data.success) {
                alert(`Checkout successful! Transaction ID: ${res.data.transaction_id}`);
                setCart([]);
                fetchProducts(); // Refresh stock
            }
        } catch (err: any) {
            alert(`Checkout failed: ${err.response?.data || err.message}`);
        }
    };

    return (
        <div className="flex flex-col lg:flex-row gap-6 h-[calc(100vh-8rem)] animate-fade-in">

            {/* Product Selection */}
            <div className="flex-1 flex flex-col gap-4 min-h-0">
                <div>
                    <h1 className="text-3xl font-bold tracking-tight mb-4">Point of Sale</h1>
                    <div className="relative">
                        <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-textMuted/50" />
                        <input
                            type="text"
                            placeholder="Search products to add..."
                            className="input-field pl-10"
                            value={search}
                            onChange={(e) => setSearch(e.target.value)}
                        />
                    </div>
                </div>

                <div className="flex-1 overflow-y-auto pr-2 grid grid-cols-2 sm:grid-cols-3 gap-4 pb-4">
                    {filteredProducts.map(p => (
                        <div
                            key={p.id}
                            onClick={() => p.stock > 0 && addToCart(p)}
                            className={`glass-panel p-4 flex flex-col justify-between transition-all duration-200 cursor-pointer ${p.stock > 0
                                    ? 'hover:scale-[1.02] hover:border-primary/50'
                                    : 'opacity-50 grayscale cursor-not-allowed'
                                }`}
                        >
                            <div>
                                <h3 className="font-semibold text-lg leading-tight mb-1">{p.name}</h3>
                                <p className="text-success font-medium">${p.price.toFixed(2)}</p>
                            </div>
                            <div className="mt-4 text-xs font-medium text-textMuted bg-surface px-2 py-1 rounded inline-flex self-start">
                                Stock: {p.stock}
                            </div>
                        </div>
                    ))}
                </div>
            </div>

            {/* Cart Ledger */}
            <div className="w-full lg:w-96 glass-panel flex flex-col h-full border-l border-border/50 shadow-2xl shrink-0">
                <div className="p-4 border-b border-border bg-surface/50 font-semibold flex items-center gap-2">
                    <ShoppingCart className="w-5 h-5 text-primary" />
                    Current Order
                </div>

                <div className="flex-1 overflow-y-auto p-4 space-y-4">
                    {cart.length === 0 ? (
                        <div className="h-full flex flex-col items-center justify-center text-textMuted opacity-50 space-y-2">
                            <ShoppingCart className="w-12 h-12" />
                            <p>Cart is empty</p>
                        </div>
                    ) : (
                        cart.map(item => (
                            <div key={item.id} className="flex items-center gap-3 bg-surface/30 p-3 rounded-xl border border-border/50">
                                <div className="flex-1 min-w-0">
                                    <h4 className="font-medium truncate">{item.name}</h4>
                                    <p className="text-sm text-textMuted">${item.price.toFixed(2)}</p>
                                </div>

                                <div className="flex items-center gap-2 bg-background rounded-lg border border-border">
                                    <button onClick={() => updateQuantity(item.id, -1)} className="p-1.5 hover:text-primary transition-colors disabled:opacity-30"><Minus className="w-4 h-4" /></button>
                                    <span className="w-6 text-center text-sm font-medium">{item.quantity}</span>
                                    <button onClick={() => updateQuantity(item.id, 1)} className="p-1.5 hover:text-primary transition-colors"><Plus className="w-4 h-4" /></button>
                                </div>

                                <button onClick={() => removeFromCart(item.id)} className="p-2 text-textMuted hover:text-danger hover:bg-danger/10 rounded-lg transition-colors ml-1">
                                    <Trash2 className="w-4 h-4" />
                                </button>
                            </div>
                        ))
                    )}
                </div>

                <div className="p-4 bg-surface/50 border-t border-border mt-auto">
                    <div className="flex justify-between items-center mb-4">
                        <span className="text-textMuted font-medium">Total Amount</span>
                        <span className="text-3xl font-bold tracking-tight text-success">${total.toFixed(2)}</span>
                    </div>

                    <div className="grid grid-cols-3 gap-2">
                        <button
                            disabled={cart.length === 0}
                            onClick={() => handleCheckout('CASH')}
                            className="flex flex-col items-center justify-center gap-1 p-3 bg-background hover:bg-surface border border-border rounded-xl transition-colors disabled:opacity-50 disabled:pointer-events-none"
                        >
                            <Banknote className="w-6 h-6 text-emerald-500" />
                            <span className="text-xs font-medium">Cash</span>
                        </button>
                        <button
                            disabled={cart.length === 0}
                            onClick={() => handleCheckout('CARD')}
                            className="flex flex-col items-center justify-center gap-1 p-3 bg-background hover:bg-surface border border-border rounded-xl transition-colors disabled:opacity-50 disabled:pointer-events-none"
                        >
                            <CreditCard className="w-6 h-6 text-blue-500" />
                            <span className="text-xs font-medium">Card</span>
                        </button>
                        <button
                            disabled={cart.length === 0}
                            onClick={() => handleCheckout('MOBILE_PAYMENT')}
                            className="flex flex-col items-center justify-center gap-1 p-3 bg-background hover:bg-surface border border-border rounded-xl transition-colors disabled:opacity-50 disabled:pointer-events-none"
                        >
                            <Smartphone className="w-6 h-6 text-purple-500" />
                            <span className="text-xs font-medium">Mobile</span>
                        </button>
                    </div>
                </div>
            </div>
        </div>
    );
}
