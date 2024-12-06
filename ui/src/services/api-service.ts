import { env } from "@/utils/env"
import { useToast } from "vue-toastification";

export async function cbFetch<T = any>(url: string, method:string = "GET", body?: any) {
    const token = localStorage.getItem(env.localStorageTokenKey) 
    const res = await fetch(`${env.apiURL}${url}`, {
        method: method,
        headers: {
            Authorization: `Bearer ${token}` 
        },
        body: JSON.stringify(body)
    })
    if(!res.ok) throw new Error(res.statusText);
    const ret = res.json() as T;
    if((ret as any).error) {
        const toast = useToast();
        toast.error((ret as any).error);
    }
    return ret;
}