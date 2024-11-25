<script setup lang="ts">
    import Nav from '@/components/Nav.vue'
    import { env } from '@/utils/env';
    import { onMounted, ref } from 'vue';

    let list = ref([]);

    onMounted(async () => {
        const token = localStorage.getItem(env.localStorageTokenKey) 
        const res = await fetch(`${env.apiURL}/api/tables`, {
            headers: {
                Authorization: `Bearer ${token}` 
            },
        })
        if(res.ok) {
            const listTable = await res.json();
            list.value = listTable.map((v:string) => {return {label:v}});
            console.log(list)
        }
    })
</script>

<template>
    <div class="main-container">
        <Nav class="secondary" :list=list></Nav>
        <div class="content">

            <table>
                <thead>
                    <tr>
                    <th>Name</th>
                    <th>Home Runs</th>
                    <th>AVG</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                    <td>Mike Trout</td>
                    <td>45</td>
                    <td>.291</td>
                    </tr>
                    <tr>
                    <td>Christian Yelich</td>
                    <td>44</td>
                    <td>.329</td>
                    </tr>
                    <tr>
                    <td>Mookie Betts</td>
                    <td>29</td>
                    <td>.295</td>
                    </tr>
                    <tr>
                    <td>Cody Bellinger</td>
                    <td>47</td>
                    <td>.305</td>
                    </tr>
                </tbody>
            </table>

        </div>
    </div>
</template>

<style scoped>
.nav.secondary {
    background-color: var(--container-color);
}
.content {
    flex-grow: 1;
    padding: 1rem;

    table {
        width: 100%;
    }
}
</style>