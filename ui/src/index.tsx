import { createContext, render } from 'preact';
import { LocationProvider, Router, Route } from 'preact-iso';

import { Home } from './pages/Home.js';
import { Login } from './pages/Login.js';
import { useMemo, useState } from 'preact/hooks';
import { ProtectedRoute } from './components/ProtectedRoute.js';
import './style.css';
import { User } from './model/user.js';

export const AuthContext = createContext(undefined);

export function App() {
	const [user, setUser] = useState<User>(undefined)

	const auth = useMemo(() => {
		return { user, setUser }
	}, [user])

	return (
		<LocationProvider>
			<AuthContext.Provider value={auth}>
			<main>
				<Router>
					<Route path="/login" component={Login} />
					<ProtectedRoute path="/" component={Home} />
					<ProtectedRoute path="/*" component={Home} />
				</Router>
			</main>
			</AuthContext.Provider>
		</LocationProvider>
	);
}

render(<App />, document.getElementById('app'));
