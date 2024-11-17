import { Router } from 'preact-iso';
import { ProtectedRoute } from '../components/ProtectedRoute';
import { Editor } from './content/Editor';
import { Tables } from './content/Tables';
import './style.css';
import { Nav } from '../components/Nav';

export function Home() {

	const style = {
		overflow: "auto",
		width: "100%",
		padding: "1rem",
	}

	return (
		<>
			<Nav></Nav>
			<div style={style}>
				<Router>
					<ProtectedRoute path="/tables" component={Tables} />
					<ProtectedRoute path="/editor" component={Editor} />
				</Router>
			</div>
		</>
	);
}
