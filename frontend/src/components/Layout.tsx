import React from 'react';
import { Outlet, Navigate, Link, useLocation, useNavigate } from 'react-router-dom';
import { LayoutDashboard, PackageOpen, ShoppingCart, Users, FolderTree, LogOut } from 'lucide-react';
import { cn } from '../lib/utils';

export function Layout() {
    const token = localStorage.getItem('token');
    const location = useLocation();
    const navigate = useNavigate();

    if (!token) {
        return <Navigate to="/login" replace />;
    }

    const handleLogout = () => {
        localStorage.removeItem('token');
        navigate('/login');
    };

    const navItems = [
        { path: '/', label: 'Dashboard', icon: LayoutDashboard },
        { path: '/products', label: 'Products', icon: PackageOpen },
        { path: '/pos', label: 'Point of Sale', icon: ShoppingCart },
        { path: '/categories', label: 'Categories', icon: FolderTree },
        { path: '/suppliers', label: 'Suppliers', icon: Users },
    ];

    return (
        <div className="min-h-screen bg-background flex">
            {/* Sidebar */}
            <div className="w-64 glass-panel border-y-0 border-l-0 rounded-none border-r border-border p-6 flex flex-col h-screen sticky top-0">
                <div className="flex items-center gap-3 mb-10 px-2">
                    <div className="w-10 h-10 bg-primary/20 rounded-xl flex items-center justify-center border border-primary/30">
                        <PackageOpen className="w-6 h-6 text-primary" />
                    </div>
                    <div>
                        <h2 className="font-bold text-lg leading-tight tracking-tight">Supermarket</h2>
                        <p className="text-xs text-textMuted">Hero Admin</p>
                    </div>
                </div>

                <nav className="flex-1 space-y-2">
                    {navItems.map((item) => {
                        const isActive = location.pathname === item.path || (item.path !== '/' && location.pathname.startsWith(item.path));
                        return (
                            <Link
                                key={item.path}
                                to={item.path}
                                className={cn(
                                    "flex items-center gap-3 px-4 py-3 rounded-xl font-medium transition-all duration-200 group",
                                    isActive
                                        ? "bg-primary text-white shadow-lg shadow-primary/20"
                                        : "text-textMuted hover:bg-surface hover:text-textMain"
                                )}
                            >
                                <item.icon className={cn("w-5 h-5", isActive ? "text-white" : "text-textMuted group-hover:text-textMain")} />
                                {item.label}
                            </Link>
                        );
                    })}
                </nav>

                <button
                    onClick={handleLogout}
                    className="flex items-center gap-3 px-4 py-3 rounded-xl font-medium text-danger hover:bg-danger/10 transition-colors mt-auto"
                >
                    <LogOut className="w-5 h-5" />
                    Sign Out
                </button>
            </div>

            {/* Main Content */}
            <div className="flex-1 p-8 lg:p-12 overflow-y-auto w-full">
                <div className="max-w-7xl mx-auto">
                    <Outlet />
                </div>
            </div>
        </div>
    );
}
