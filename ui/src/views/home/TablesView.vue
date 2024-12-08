<script setup lang="ts">

import {
    Dialog,
    DialogTrigger,
} from '@/components/ui/dialog'
import { Input } from '@/components/ui/input';
import { Button } from '@/components/ui/button';
import {
    Table,
    TableBody,
    TableCell,
    TableHead,
    TableHeader,
    TableRow,
} from '@/components/ui/table'
import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger,
} from '@/components/ui/tooltip'

import { Plus, Users, Folder, Play, Settings } from 'lucide-vue-next'
import  InsertRow from "@/components/InsertRow.vue"
import { onMounted, ref } from 'vue';
import CreateTable from "@/components/CreateTable.vue";
import { cbFetch } from "@/services/api-service";

const list = ref<string[]>([]);
const search = ref<string>("");


const currentTable = ref<string>("user");

const error = ref<any>(undefined);
const cols = ref<string[]>([]);
const data = ref<{ [key: string]: string }[]>([]);

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
    <div class="flex flex-row h-full">
        <div class="flex flex-col rounded-tr-2xl p-4 gap-4 items-center bg-secondary text-secondary-foreground">
            <span class="flex flex-row gap-4 items-center cursor-pointer"
                v-for="table of list.filter((t: string) => t.includes(search))" @click="queryTable(table)">
                <Users v-if="table == 'user'" />
                <Folder v-else />
                {{ table }}
            </span>

            <Dialog>
                <DialogTrigger>
                    <TooltipProvider>
                        <Tooltip>
                            <TooltipTrigger as-child>
                                <Plus />
                            </TooltipTrigger>
                            <TooltipContent>
                                <p>Create a new table</p>
                            </TooltipContent>
                        </Tooltip>
                    </TooltipProvider>
                </DialogTrigger>
                <CreateTable></CreateTable>
            </Dialog>



        </div>

        <div class="flex gap-4 flex-col pt-0 p-4 w-full">

            <template v-if="cols && cols.length > 0">
                <div class="flex w-full items-center gap-1.5">

                    <Dialog>
                        <DialogTrigger>
                    <TooltipProvider>
                        <Tooltip>
                            <TooltipTrigger as-child>
                                <Button variant="outline">
                                    <Settings />
                                </Button>
                            </TooltipTrigger>
                            <TooltipContent>
                                <p>Edit table</p>
                            </TooltipContent>
                        </Tooltip>
                    </TooltipProvider>
                </DialogTrigger>
            </Dialog>


            <Dialog>
                        <DialogTrigger>
                    <TooltipProvider>
                        <Tooltip>
                            <TooltipTrigger as-child>
                                <Button variant="outline">
                                    <Plus />
                                </Button>
                            </TooltipTrigger>
                            <TooltipContent>
                                <p>Insert a row</p>
                            </TooltipContent>
                        </Tooltip>
                    </TooltipProvider>
                </DialogTrigger>
                <InsertRow :table-name="currentTable" :cols="cols"></InsertRow>
            </Dialog>


                    <Input type="text" placeholder="Where clause" />
                    <Button variant="outline">
                        <Play />
                    </Button>
                </div>

                <Table>
                    <TableHeader>
                        <TableRow>
                            <TableHead v-for="col of cols">
                                {{ col }}
                            </TableHead>
                        </TableRow>
                    </TableHeader>
                    <TableBody>
                        <TableRow v-for="d of data">
                            <TableCell v-for="col of cols">
                                {{ d[col] }}
                            </TableCell>
                        </TableRow>
                    </TableBody>
                </Table>
            </template>
            <pre v-else-if="error" class="error">{{ error }}</pre>
        </div>

    </div>
</template>

<style scoped></style>