import { createContext, render } from 'preact';
import { LocationProvider, Router, Route } from 'preact-iso';

import { Home } from './pages/Home.js';
import { Login } from './pages/Login.js';
import { useMemo, useState } from 'preact/hooks';
import { ProtectedRoute } from './components/ProtectedRoute.js';
import './style.css';

export const AuthContext = createContext(undefined);

export function App() {
	const [token, setToken] = useState<string>("")

	const auth = useMemo(() => {
		return { token, setToken }
	}, [token])

	return (
		<LocationProvider>
			<AuthContext.Provider value={auth}>
				<Router>
					<Route path="/login" component={Login} />
					<ProtectedRoute path="/*" component={Home} />
					<ProtectedRoute path="/" component={Home} />
				</Router>
			</AuthContext.Provider>
		</LocationProvider>
	);
}

render(<App />, document.getElementById('app'));
