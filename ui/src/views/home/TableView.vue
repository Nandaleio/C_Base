<script setup lang="ts">
import { env } from '@/utils/env';
import { onMounted, ref, useTemplateRef } from 'vue';
import CreateTable from "@/components/CreateTable.vue"

const list = ref<string[]>([]);
const search = ref("");

const error = ref<any>();
const cols = ref<string[]>([]);
const data = ref<{ [key: string]: string }[]>([]);


const createTableOpen = useTemplateRef('create-table')

onMounted(async () => {
    const token = localStorage.getItem(env.localStorageTokenKey)
    const res = await fetch(`${env.apiURL}/api/tables`, {
        headers: {
            Authorization: `Bearer ${token}`
        },
    })
    if (res.ok) {
        const listTable = await res.json();
        list.value = listTable.data.map((v: any) => { return v.name });
        console.log(list)
    }
})

async function queryTable(table: string) {
    const token = localStorage.getItem(env.localStorageTokenKey)
    const res = await fetch(`${env.apiURL}/api/tables/${table}`, {
        headers: {
            Authorization: `Bearer ${token}`
        },
    })
    if (res.ok) {
        const { columns, data, error } = await res.json();
        cols.value = columns;
        data.value = data;
        error.value = error;
    }
}

</script>

<template>
    <div class="main-container">
        <div class="list-table">
            <!-- <input type="search" name="search" v-model="search" placeholder="Search" aria-label="Search" /> -->
            <span class="item-table" v-for="table of list.filter(t => t.includes(search))" @click="queryTable(table)">
                <span class="material-symbols-outlined">
                    {{ table == "user" ? 'group' : 'folder' }}
                </span>
                {{ table }}
            </span>

            <button class="small outline" @click="createTableOpen!.open()" data-tooltip="Create a new table"
                data-placement="right">
                <span class="material-symbols-outlined">
                    add
                </span>
            </button>
        </div>

        <div class="content">
            <template v-if="cols && cols.length">
                <table>
                    <thead>
                        <tr>
                            <th v-for="col of cols">
                                {{ col }}
                            </th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr v-for="d of data">
                            <td v-for="col of cols"">
                                {{ d[col] }}
                            </td>
                        </tr>
                    </tbody>
                </table>
            </template>
            <pre v-else-if="error" class="error">{{ error }}</pre>
            <CreateTable ref="create-table"></CreateTable>
        </div>
    </div>
</template>

<style scoped>
.list-table {

    display: flex;
    flex-direction: column;
    align-items: stretch;
    gap: 1rem;

    padding: 1rem;
    background-color: var(--container-color);

    .item-table {
        cursor: pointer;
        display: flex;
        align-items: center;
        gap: .5rem;
    }
}

.content {
    flex-grow: 1;
    padding: 1rem;

    table {
        width: 100%;
    }
}

.small {
    display: flex;
    justify-content: center;

    padding: 0.5rem 0.35rem;
}
</style>