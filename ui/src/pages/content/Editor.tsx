import { useContext, useState } from "preact/hooks"
import { AuthContext } from "../.."


export function Editor() {

    const {token} = useContext(AuthContext);

    const [value, setValue] = useState('')
    const [result, setResult] = useState<{data: any[], error: string, msg: string}>()

    const onInput = e => {
        setValue(e.currentTarget.value);
    }
      
    const runCommand = async (event: Event): Promise<void> => {
        const res = await fetch("http://localhost:8080/api/tables", {
            method: 'post',
            headers: new Headers({
                'Authorization': `Basic ${token}`,
                'Accept': 'application/json',
                'Content-Type': 'application/json'
            }),
            body: JSON.stringify({query: value})
        })
        console.log(await res.json());
    }

    const renderResult = () => {
        if(result?.error) 
            return <pre style="color:red"> {result?.error}</pre>
        else if (result?.msg)
            return <pre>{result.msg}</pre>
        else if (result?.data) 
            return <table>
                <thead>
                    <tr> 
                        {Object.keys(result?.data[0]).map(function(k){
                            return <th>{k}</th>;
                        })}
                    </tr>
                </thead>
                <tbody>
                    {result?.data.map(function(row){
                        return <tr>
                            {Object.values(row).map(function(v){
                            return <th>{v as string}</th>;
                            })}
                        </tr>;
                    })}
                </tbody>
            </table>
        else 
            return <></>
    }

    return(
        <div class="editor">
            <div class="input">
                <div>
                    <button onClick={runCommand}>▶</button>
                </div>
                <textarea style="width: 100%; height: 45vh;" onInput={onInput}></textarea>
            </div>
            <div class="result">
                {renderResult()}
            </div>
        </div>
    )
}