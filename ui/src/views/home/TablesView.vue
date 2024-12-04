<script setup lang="ts">
    import { onMounted, ref, useTemplateRef } from 'vue';
    import CreateTable from "@/components/CreateTable.vue";
    import { cbFetch } from "@/services/api-service";

    const list = ref<string[]>([]);
    const search = ref<string>("");

    const error = ref<any>(undefined);
    const cols = ref<string[]>([]);
    const data = ref<{ [key: string]: string }[]>([]);

    const createTableOpen = useTemplateRef('create-table');

    async function queryTable(table: string) {
        const res = await cbFetch(`/api/table/${table}`)
        cols.value = res.columns;
        data.value = res.data;
        error.value = res.error;
    }

    onMounted(async () => {
        const res = await cbFetch('/api/tables');
        list.value = res.data.map((v: any) => { return v.name });
    })

</script>

<template>
    <div class="main-container">
        <div class="list-table">
            <span class="item-table" v-for="table of list.filter((t:string) => t.includes(search))" @click="queryTable(table)">
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
                            <td v-for="col of cols">
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