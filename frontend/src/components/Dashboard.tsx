import { useEffect, useState } from 'react';
import { PackageOpen, AlertTriangle, TrendingUp, DollarSign } from 'lucide-react';
import api from '../lib/api';

export function Dashboard() {
    const [stats, setStats] = useState({
        total_products: 0,
        low_stock_items: 0,
        out_of_stock_items: 0,
        total_inventory_value: 0
    });
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        const fetchStats = async () => {
            try {
                const res = await api.get('/inventory/stats');
                setStats(res.data);
            } catch (err) {
                console.error("Failed to fetch stats", err);
            } finally {
                setLoading(false);
            }
        };
        fetchStats();
        const interval = setInterval(fetchStats, 10000);
        return () => clearInterval(interval);
    }, []);

    const statsCards = [
        { label: 'Total Products', value: stats.total_products, icon: PackageOpen, color: 'text-primary', bg: 'bg-primary/10', border: 'border-primary/20' },
        { label: 'Inventory Value', value: `$${stats.total_inventory_value.toFixed(2)}`, icon: DollarSign, color: 'text-success', bg: 'bg-success/10', border: 'border-success/20' },
        { label: 'Low Stock Items', value: stats.low_stock_items, icon: AlertTriangle, color: 'text-warning', bg: 'bg-warning/10', border: 'border-warning/20' },
        { label: 'Out of Stock', value: stats.out_of_stock_items, icon: TrendingUp, color: 'text-danger', bg: 'bg-danger/10', border: 'border-danger/20' }
    ];

    return (
        <div className="space-y-6 animate-fade-in">
            <div className="flex items-center justify-between mb-8">
                <div>
                    <h1 className="text-3xl font-bold tracking-tight">Dashboard Overview</h1>
                    <p className="text-textMuted mt-1">Real-time inventory statistics</p>
                </div>
            </div>

            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
                {statsCards.map((s, idx) => (
                    <div key={idx} className={`glass-panel p-6 border ${s.border} hover:scale-[1.02] transition-transform duration-300`}>
                        <div className="flex items-center justify-between mb-4">
                            <div className={`w-12 h-12 rounded-xl flex items-center justify-center ${s.bg}`}>
                                <s.icon className={`w-6 h-6 ${s.color}`} />
                            </div>
                        </div>
                        <div>
                            <p className="text-sm font-medium text-textMuted">{s.label}</p>
                            <h3 className="text-3xl font-bold mt-1 tracking-tight">
                                {loading ? <span className="animate-pulse bg-surface/50 text-transparent rounded">0000</span> : s.value}
                            </h3>
                        </div>
                    </div>
                ))}
            </div>
        </div>
    );
}
